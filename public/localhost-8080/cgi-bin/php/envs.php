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
?>
<html>
<head>
    <title>CGI Test Script</title>
</head>
<body>
    <h1>CGI Environment Variables</h1>
    <h2>Environment Variables</h2>
        <ul>
        <?php foreach ($_SERVER as $key => $value) {
            echo "<li>";
            echo "<strong>$key:</strong>";
            echo (is_array($value) ? implode(", ", $value) : $value);
            echo "</li>";
        } ?>
        </ul>
    <h2>Form Data</h2>"

<?php
$method = $_SERVER['REQUEST_METHOD'];
?>
    <h3><? $method ?> Data</h3>
        <ul>

<?php
if ($method == 'POST' && !empty($_POST)) {
	$post_data = $_POST;
    foreach ($post_data as $key => $value) {
        echo "<li><strong>$key:</strong> $value</li>";
    }
} else if (!empty($_SERVER['QUERY_STRING'])) {
	$query_string = $_SERVER['QUERY_STRING'];
    parse_str($query_string, $params);
    foreach ($params as $key => $values) {
        foreach ((array)$values as $value) {
            echo "<li><strong>$key:</strong> $value</li>";
        }
    }
}
?>
        </ul>
    <h2>Cookies</h2>
        <?php print_cookies(); ?>
</body>
</html>
