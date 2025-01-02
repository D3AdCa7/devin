<?php
class WhmcsApi {
    private $url;
    private $identifier;
    private $secret;

    public function __construct() {
        require_once 'config.php';
        $this->url = WHMCS_URL;
        $this->identifier = WHMCS_IDENTIFIER;
        $this->secret = WHMCS_SECRET;
    }

    private $testMode = true; // Enable test mode for development

    private function getMockResponse($action, $params) {
        if ($action === 'ValidateLogin') {
            if ($params['email'] === 'test@example.com' && $params['password2'] === 'testpass') {
                return [
                    'result' => 'success',
                    'userid' => 123
                ];
            }
            return [
                'result' => 'error',
                'message' => 'Invalid email or password'
            ];
        } elseif ($action === 'GetClientsDetails') {
            if ($params['clientid'] === 123) {
                return [
                    'result' => 'success',
                    'client' => [
                        'firstname' => 'John',
                        'lastname' => 'Doe',
                        'email_verified' => true
                    ]
                ];
            }
            return [
                'result' => 'error',
                'message' => 'Client not found'
            ];
        }
        return ['result' => 'error', 'message' => 'Invalid action'];
    }

    private function callApi($action, $params) {
        if ($this->testMode) {
            return $this->getMockResponse($action, $params);
        }

        $params['action'] = $action;
        $params['identifier'] = $this->identifier;
        $params['secret'] = $this->secret;
        $params['responsetype'] = 'json';

        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $this->url);
        curl_setopt($ch, CURLOPT_POST, 1);
        curl_setopt($ch, CURLOPT_POSTFIELDS, http_build_query($params));
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 1);
        curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 2);
        
        $response = curl_exec($ch);
        if (curl_errno($ch)) {
            throw new Exception('CURL Error: ' . curl_error($ch));
        }
        curl_close($ch);

        return json_decode($response, true);
    }

    private function getClientDetails($userId) {
        $params = array(
            'clientid' => $userId,
            'stats' => false
        );

        try {
            $response = $this->callApi('GetClientsDetails', $params);
            if ($response['result'] === 'success' && isset($response['client'])) {
                return [
                    'success' => true,
                    'details' => [
                        'id' => $params['clientid'],
                        'firstName' => $response['client']['firstname'],
                        'lastName' => $response['client']['lastname'],
                        'emailVerified' => $response['client']['email_verified']
                    ]
                ];
            }
            return [
                'success' => false,
                'message' => 'Failed to retrieve user details'
            ];
        } catch (Exception $e) {
            return [
                'success' => false,
                'message' => 'System error occurred while retrieving user details'
            ];
        }
    }

    public function validateLogin($email, $password) {
        $params = array(
            'email' => $email,
            'password2' => $password
        );

        try {
            $response = $this->callApi('ValidateLogin', $params);
            if ($response['result'] === 'success' && isset($response['userid'])) {
                $userDetails = $this->getClientDetails($response['userid']);
                if ($userDetails['success']) {
                    return [
                        'success' => true,
                        'message' => 'Login successful',
                        'user' => $userDetails['details']
                    ];
                }
                // If getting user details fails, still return successful login but with error message
                return [
                    'success' => true,
                    'message' => 'Login successful but ' . $userDetails['message']
                ];
            }
            return [
                'success' => false,
                'message' => 'Invalid email or password'
            ];
        } catch (Exception $e) {
            return [
                'success' => false,
                'message' => 'System error occurred during login'
            ];
        }
    }

    public function addClient($userData) {
        $requiredFields = ['firstname', 'lastname', 'email', 'password', 'address1', 
                          'city', 'state', 'postcode', 'country', 'phonenumber'];
        
        // Validate required fields
        foreach ($requiredFields as $field) {
            if (empty($userData[$field])) {
                return [
                    'success' => false,
                    'message' => "Missing required field: $field"
                ];
            }
        }

        // Map the password field to WHMCS expected field name
        $userData['password2'] = $userData['password'];
        unset($userData['password']);

        try {
            $response = $this->callApi('AddClient', $userData);
            return [
                'success' => ($response['result'] === 'success'),
                'message' => ($response['result'] === 'success') ? 'Registration successful' : $response['message'],
                'clientId' => isset($response['clientid']) ? $response['clientid'] : null
            ];
        } catch (Exception $e) {
            return [
                'success' => false,
                'message' => 'System error occurred during registration'
            ];
        }
    }
}
