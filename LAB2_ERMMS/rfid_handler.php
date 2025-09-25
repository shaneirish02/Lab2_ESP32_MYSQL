<?php
include "db_config.php";

if (isset($_POST['rfid_data'])) {
    $rfid = $_POST['rfid_data'];

    // Search in rfid_reg
    $sql = "SELECT * FROM rfid_reg WHERE rfid_data='$rfid'";
    $result = $conn->query($sql);

    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        $status = $row['rfid_status'];

        // Toggle status
        $newStatus = $status == 1 ? 0 : 1;

        // Update status in reg
        $conn->query("UPDATE rfid_reg SET rfid_status=$newStatus WHERE rfid_data='$rfid'");

        // Insert log
        $time = date("Y-m-d h:i:s A");
        $conn->query("INSERT INTO rfid_logs (time_log, rfid_data, rfid_status) 
                      VALUES ('$time', '$rfid', $newStatus)");

        echo $status == 1 ? "0" : "1"; // return for ESP32
    } else {
        // Insert log if not found
        $time = date("Y-m-d h:i:s A");
        $conn->query("INSERT INTO rfid_logs (time_log, rfid_data, rfid_status) 
                      VALUES ('$time', '$rfid', 0)");

        echo "RFID NOT FOUND";
    }
} else {
    echo "No RFID received";
}
$conn->close();
?>
