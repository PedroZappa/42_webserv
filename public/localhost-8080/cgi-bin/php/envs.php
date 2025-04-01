#!/usr/bin/env php

<?php

$method = $_SERVER['REQUEST_METHOD'];

function print_cookies() {
	if (empty($_SERVER['HTTP_COOKIE'])) {
        echo "No cookies to show.";
        return;
    }

    $cookies = $_SERVER['HTTP_COOKIE'];
	$params = explode('; ', $cookies);
	foreach ($params as $value) {
        $av = explode('=', $value);
		$key = $av[0];
		$value = $av[1];
		echo "<li><strong>$key</strong>=$value</li>";
	}
}

function print_post_data() {
    $method = $_SERVER['REQUEST_METHOD'];
    if ($method != 'POST') return;

    if (empty($_POST)) {
        echo "No data to show.";
        return;
    }

    $post_data = $_POST;
    foreach ($post_data as $key => $value) {
        echo "<li><strong>$key:</strong> $value</li>";
    }
}

function print_get_data() {
    $method = $_SERVER['REQUEST_METHOD'];
    if ($method != 'GET') return;
    if (empty($_SERVER['QUERY_STRING'])) {
        echo "No data to show.";
        return;
    }
    
    $query_string = $_SERVER['QUERY_STRING'];
    parse_str($query_string, $params);
    foreach ($params as $key => $values)
        foreach ((array)$values as $value)
            echo "<li><strong>$key:</strong> $value</li>";       
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
    <h2>Form Data</h2>
    <h3><?php echo $method; ?> Data</h3>
        <ul>
            <?php print_get_data(); ?>
            <?php print_post_data(); ?>
        </ul>
    <h2>Cookies</h2>
        <?php print_cookies(); ?>
</body>
</html>
