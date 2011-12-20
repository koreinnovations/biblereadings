<?php

function biblereading_load($id) {
  return db_fetch_object(db_query("SELECT * from {biblereadings_readings} WHERE id = %d", $id));
}

function biblereadings_readings_by_date_range($startdate, $enddate) {
  $items = array();
  $sql = "SELECT r.*, b.name as bookname, b.abbreviation
      FROM {biblereadings_readings} r
      JOIN {biblereadings_books} b
        ON b.id = r.startbook
      WHERE date >= %d and date <= %d
      ORDER BY date ASC";

  $q = db_query($sql, $startdate, $enddate);

  while ($i = db_fetch_object($q)) {
    $items[] = $i;
  }
  return $items;
}

function biblereadings_readings_this_week() {
  $fullday = 24 * 60 * 60;
  $day = date('w');
  $today = strtotime(date('m/d/Y'));
  $sunday = $today - ($day * $fullday);
  $saturday = $today + ((6 - $day) * $fullday);

  $readings = biblereadings_readings_by_date_range($sunday, $saturday);

  return $readings;
}

function biblereadings_all_readings() {
  
}

function biblereadings_get_today_psalms($date) {
  $d = date('d', $date);
  if ($d != 31) {
    $endpsalm = $d * 5;
    $startpsalm = $endpsalm - 4;
    return array($startpsalm, $endpsalm);
  } else {
    return array(119);
  }
}

function biblereadings_get_today_proverb($date) {
  $d = date('j', $date);
  return $d;
}