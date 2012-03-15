<?php

function biblereadings_get_subscribers($active_only = TRUE) {
  $query = "SELECT * FROM {biblereadings_subscriptions}";
  if ($active_only) {
    $query .= ' WHERE unsubscribed = 0';
  }
  $result = db_query($query);

  $items = array();

  foreach ($result as $item) {
    $items[] = array(
        'name' => $item->name,
        'email' => $item->email,
        'joined' => $item->joined,
    );
  }

  return $items;
}

function biblereadings_format_regular_reading($reading, $abbreviated = FALSE, $link = FALSE, $version = 'NKJV') {
  if ($abbreviated) {
    $regular = $reading->abbreviation;
  }
  else {
    $regular = $reading->bookname;
  }
  if ($reading->startchapter) {
    $regular .= sprintf(' %s-%s', $reading->startchapter, $reading->endchapter);
  }

  if ($link) {
    $url = sprintf('http://www.biblegateway.com/passage/?search=%s&version=%s', urlencode($regular), $version);
    $regular = l($regular, $url, array('attributes' => array('target' => '_blank')));
  }

  return $regular;
}

function biblereadings_format_proverb($reading, $abbreviated = FALSE, $link = FALSE, $version = 'NKJV') {
  if ($abbreviated) {
    $pr = t('Pr');
  }
  else {
    $pr = t('Proverbs');
  }
  $proverb = sprintf('%s %d', $pr, $reading->proverb);

  if ($link) {
    $url = sprintf('http://www.biblegateway.com/passage/?search=%s&version=%s', urlencode($proverb), $version);
    $proverb = l($proverb, $url, array('attributes' => array('target' => '_blank')));
  }

  return $proverb;
}

function biblereadings_format_psalms($reading, $abbreviated = FALSE, $link = FALSE, $version = 'NKJV') {
  if ($reading->psalms[1]) {
    if ($abbreviated) {
      $ps = t('Ps');
    }
    else {
      $ps = t('Psalms');
    }

    $psalms = sprintf("%s %d-%d", $ps, $reading->psalms[0], $reading->psalms[1]);
  }
  else {
    if ($abbreviated) {
      $ps = t('Ps');
    }
    else {
      $ps = t('Psalm');
    }

    $psalms = sprintf("%s %d", $ps, $reading->psalms[0]);
  }

  if ($link) {
    $url = sprintf('http://www.biblegateway.com/passage/?search=%s&version=%s', urlencode($psalms), $version);
    $psalms = l($psalms, $url, array('attributes' => array('target' => '_blank')));
  }

  return $psalms;
}

function biblereading_load($id) {
  return db_query("SELECT * from {biblereadings_readings} WHERE id = :id", array(':id' => $id))->fetchObject();
}

function biblereadings_readings_by_date($date) {

  $start_date = strtotime(date('m/d/Y', $date));
  $end_date = strtotime(date('m/d/Y', $date)) + ONE_DAY - 1;

  $items = biblereadings_readings_by_date_range($start_date, $end_date);

  return $items[0];
}

function biblereadings_readings_by_date_range($startdate, $enddate) {
  $items = array();

  $q = db_query("SELECT r.*, b.name as bookname, b.abbreviation
      FROM {biblereadings_readings} r
      JOIN {biblereadings_books} b
        ON b.id = r.startbook
      WHERE date >= :startdate and date <= :enddate
      ORDER BY date ASC", array(':startdate' => $startdate, ':enddate' => $enddate));

  foreach ($q as $i) {
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

function biblereadings_subscription_exists($email) {
  $query = "SELECT COUNT(*) FROM {biblereadings_subscriptions} WHERE email = :email";
  $result = db_query($query, array(':email' => $email));
  
  return ($result->fetchField() > 0);
}

function biblereadings_unsubscribe($email) {
  $query = "UPDATE {biblereadings_subscriptions} SET unsubscribed = :unsubscribed WHERE email = :email";
  db_query($query, array(':email' => $email, ':unsubscribed' => time()));
}