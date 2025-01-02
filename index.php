<?php
require_once 'config.php';
require_once 'WhmcsApi.php';

// Initialize WHMCS API
$whmcsApi = new WhmcsApi();

// Check request method first
if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo json_encode([
        'success' => false,
        'message' => 'Method not allowed. Only POST requests are accepted.'
    ]);
    exit;
}

// Parse JSON request body
$requestData = json_decode(file_get_contents('php://input'), true);
if (!$requestData) {
    http_response_code(400);
    echo json_encode([
        'success' => false,
        'message' => 'Invalid JSON request body'
    ]);
    exit;
}
if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo json_encode([
        'success' => false,
        'message' => 'Method not allowed. Only POST requests are accepted.'
    ]);
    exit;
}

// Get the request path
$requestPath = parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH);
$endpoint = trim(str_replace('/whmcs-api', '', $requestPath), '/');

// Handle different endpoints
switch ($endpoint) {
    case 'login':

        // Validate required fields
        if (empty($requestData['email']) || empty($requestData['password'])) {
            http_response_code(400);
            echo json_encode([
                'success' => false,
                'message' => 'Email and password are required'
            ]);
            break;
        }

        // Call WHMCS login validation with enhanced user details
        $result = $whmcsApi->validateLogin(
            $requestData['email'],
            $requestData['password']
        );

        if (!$result['success']) {
            http_response_code(401);
        }
        
        // Set content type and encode response
        header('Content-Type: application/json');
        echo json_encode($result);
        break;

    case 'register':

        // Call WHMCS client registration
        $result = $whmcsApi->addClient($requestData);
        
        if (!$result['success']) {
            http_response_code(400);
        }
        echo json_encode($result);
        break;

    default:
        http_response_code(404);
        echo json_encode([
            'success' => false,
            'message' => 'Endpoint not found'
        ]);
        break;
}
