<?php
// WHMCS API Configuration
define('WHMCS_URL', 'https://your-whmcs-instance.com/includes/api.php');
define('WHMCS_IDENTIFIER', ''); // Your WHMCS API Identifier
define('WHMCS_SECRET', ''); // Your WHMCS API Secret

// Error reporting for development (disable in production)
error_reporting(E_ALL);
ini_set('display_errors', 1);

// API Response headers
header('Content-Type: application/json');
