<?php

function biblereading_load($id) {
  return db_query("SELECT * from {biblereadings_readings} WHERE id = :id", array(':id' => $id))->fetchObject();
}

function biblereadings_readings_by_date_range($startdate, $enddate) {
  $items = array();

  $q = db_query("SELECT r.*, b.name as bookname, b.abbreviation
      FROM {biblereadings_readings} r
      JOIN {biblereadings_books} b
        ON b.id = r.startbook
      WHERE date >= :startdate and date <= :enddate
      ORDER BY date ASC", array(':startdate' => $startdate, ':enddate' => $enddate));

  foreach($q as $i) {
    $i->psalms = biblereadings_get_today_psalms($i->date);
    $i->proverb = biblereadings_get_today_proverb($i->date);
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


function biblereadings_get_today_psalms($date) {
  $d = date('d', $date);
  if ($d != 31) {
    $endpsalm = $d * 5;
    $startpsalm = $endpsalm - 4;
    return array($startpsalm, $endpsalm);
  }
  else {
    return array(119);
  }
}

function biblereadings_get_today_proverb($date) {
  $d = date('j', $date);
  return $d;
}
