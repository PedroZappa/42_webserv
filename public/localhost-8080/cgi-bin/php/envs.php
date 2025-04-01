#!/usr/bin/env php

<?php

function print_cookies() {
	if (empty($_SERVER['HTTP_COOKIE'])) return;

	$cookies = $_SERVER['HTTP_COOKIE'];
	$params = explode('; ', $cookies);
	foreach ($params as $value) {
        $av = explode('=', $value);
		$key = $av[0];
		$value = $av[1];
		echo "<li><strong>$key</strong>=$value</li>";
	}
}

echo "Content-Type: text/html";
echo"\r\n\r\n";

echo "<html>";
echo "<head>";
echo "  <title>CGI Test Script</title>";
echo "</head>";
echo "<body>";
echo "  <h1>CGI Environment Variables</h1>";

echo "  <h2>Environment Variables</h2>";
echo "      <ul>";
foreach ($_SERVER as $key => $value) {
    echo "      <li><strong>$key:</strong> ";
    echo (is_array($value) ? implode(", ", $value) : $value);
    echo "      </li>";
}
echo "      </ul>";

echo "  <h2>Form Data</h2>";

$method = $_SERVER['REQUEST_METHOD'];
echo "  <h3>$method Data</h3>";

if ($method == 'POST' && !empty($_POST)) {
    echo "<ul>";
	$post_data = $_POST;
    foreach ($post_data as $key => $value) {
        echo "<li><strong>$key:</strong> $value</li>";
    }
} else if (!empty($_SERVER['QUERY_STRING'])){
    echo "<ul>";
	$query_string = $_SERVER['QUERY_STRING'];
    parse_str($query_string, $params);
    foreach ($params as $key => $values) {
        foreach ((array)$values as $value) {
            echo "<li><strong>$key:</strong> $value</li>";
        }
    }
    echo "</ul>";
}

echo "  <h2>Cookies</h2>";
print_cookies();

echo "</body>";
echo "</html>";
?>
