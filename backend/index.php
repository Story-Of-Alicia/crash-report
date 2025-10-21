<?php

/* generate_id($len): generates a random string of $len size
composed of alphanumeric characters, both lowercase and uppercase */
function generate_id($len) {
    $characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    $string = [];
    for($i = 0; $i < $len; $i++) {
        $string[$i] = $characters[rand(0, strlen($characters) - 1)];
    }
    return join('', $string);
}

/* validate_id($id): returns true if the string provided
is composed only of alphanumeric characters */
function validate_id($id) {
   return ctype_alnum($id);
}

/* handle_report():
Generates a unique report ID, creates it's directory
in reports/

This http handler could be used as means of regulating the traffic.
*/
function handle_report()
{
    $id = "null";
    do {
        $id = generate_id(26);
    } while (file_exists("reports/" . $id));

    if($id == "null") {
        fprintf(STDERR, "Failed to generate id");
        http_response_code(500);
        die();
    }

    if(!mkdir("reports/" . $id . "/", 0764, true)) {
        fprintf(STDERR, "Unable to to create directory 'reports/%s'", $id);
        http_response_code(500);
        die();
    }

    echo $id;

    http_response_code(200);
    die();
}

/* upload($id, $file):
Makes sure $id is correct and not older than 15 minutes.
Checks if $file is one of the following: stack.deflate, dump.deflate
If everything is in order then the file will be saved to disk
from php://input
*/
function handle_upload($id, $file) {
    if(!validate_id($id)) {
        http_response_code(400);
        die();
    }

    switch($file) {
        case 'stack.deflate': {
            break;
        }
        case 'dump.deflate': {
           break;
        }
        default: {
            http_response_code(400);
            die();
        }
    }

    if(time()-filemtime("reports/$id") > 60 * 15) {
        /* if file is older than 15 minutes return bad request */
        http_response_code(400);
        die();
    }

    $fin = fopen("php://input", "r");
    $fout = fopen("reports/" . $id . '/' . $file, "w");
    if(!chmod("reports/" . $id . '/' . $file, 0644)) {
        fprintf(STDERR, "Unable to set file permissions 'reports/%s/%s", $id, $file);
        die();
    }

    if($fout == null) {
        fprintf(STDERR, "Unable to open file 'reports/%s/%s for writing!", $id, $file);
        http_response_code(500);
        die();
     }

    while ($buf = fread($fin, 5120)) {
       if(fwrite($fout, $buf) == false) {
           http_response_code(500);
           die();
       }
    }

    fclose($fout);
    fclose($fin);

    http_response_code(200);
    die();
}

$params = preg_split('/\//', $_SERVER['REQUEST_URI'], -1, PREG_SPLIT_NO_EMPTY);

if($params[0] == "upload") {
    if(count($params) != 3) {
        http_response_code(400);
        die();
    }

    if($_SERVER['REQUEST_METHOD'] != "PUT") {
        http_response_code(405);
        die();
    }

    if($_SERVER['CONTENT_TYPE'] != "application/octet-stream") {
        http_response_code(400);
        die(400);
    }

    handle_upload($params[1], $params[2]);

    http_response_code(500);
    die();
}

if($params[0] == "report") {
    if(count($params) != 1) {
        http_response_code(400);
        die();
    }

    if($_SERVER['REQUEST_METHOD'] != "GET") {
        http_response_code(405);
        die();
    }
    handle_report();
    http_response_code(500);
    die();
}

http_response_code(404);
die();
