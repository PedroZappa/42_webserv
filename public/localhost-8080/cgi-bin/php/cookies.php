#!/usr/bin/env php

<?php

echo "Content-Type: text/html";
echo "\r\n";

if (!empty($_SERVER['QUERY_STRING'])){
	$query_string = $_SERVER['QUERY_STRING'];
    parse_str($query_string, $params);
    foreach ($params as $key => $values) {
        foreach ((array)$values as $value) {
            echo "Set-Cookie: $key=$value; HttpOnly";
			echo "\r\n";
        }
	}
}

echo "\r\n";

$cookie_name = 'name';

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
?>
<html>
<body>
	<h1>Current cookies</h1>
	<ul>
		<?php print_cookies(); ?>
	</ul>
</body>
</html> 