<?php

function biblereadings_admin_settings_form(&$form_state) {
  $form = array();

  $form['biblereadings_standard_readings_download_path'] = array(
      '#type' => 'textfield',
      '#title' => t('Standard readings download path'),
      '#default_value' => variable_get('biblereadings_standard_readings_download_path', ''),
  );
  
  $form['biblereadings_chronological_readings_download_path'] = array(
      '#type' => 'textfield',
      '#title' => t('Chronological readings download path'),
      '#default_value' => variable_get('biblereadings_chronological_readings_download_path', ''),
  );
  
  return system_settings_form($form);
}



function biblereadings_admin_import_text_form(&$form_state) {
  $form = array();

  $form['year'] = array(
      '#type' => 'textfield',
      '#title' => t('Year'),
      '#default_value' => $form_state['storage']['year'],
  );

  $form['readings_raw'] = array(
      '#type' => 'textarea',
      '#title' => t('Paste raw readings here'),
      '#default_value' => $form_state['storage']['readings_raw'],
      '#rows' => 20
  );

  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Import these readings')
  );

  return $form;
}

function biblereadings_admin_import_text_form_submit($form, &$form_state) {
  $month_re = '/^\w+$/';
  $books = biblereadings_get_books();

  //dpm($books);

  $current_month = '';
  //$file = fopen('bible readings', 'r');

  $lines = explode("\n", $form_state['values']['readings_raw']);

  $line_re = '/^\s*(\d+)\s+((((\d+\s+)*([a-zA-Z ]+))\/?)+)\s*(([0-9]+)?\s*(\d+)?)$/';

  $records = array();
  $goofy = array();

  $count = 0;
  foreach ($lines as $line) {
    $line = trim($line);
    //while($line = trim(fgets($file))) {
    if (preg_match($month_re, $line)) {
      $current_month = $line;
    } else {
      $matches = explode('#', $line);

      list($day, $books_str, $start_chapter, $end_chapter) = array($matches[0], $matches[1], $matches[2], $matches[3]);

      $books_arr = explode('/', $books_str);
      $month = date('n', strtotime($current_month));

      if (sizeof($books_arr) > 1) {
        foreach ($books_arr as $book) {
          $date = mktime(0, 0, 0, $month, $day, (int)$form_state['values']['year']);
          $book = trim(strtolower($book));
          $o = new stdClass();
          $o->date = $date;
          $o->endbook = $o->startbook = $books[$book]->id;
          $o->startchapter = 1;
          $o->endchapter = $books[$book]->chapters;
          drupal_write_record('biblereadings_readings', $o);
          $count++;
          //$records[] = $o;
        }
      } else {
        $book = trim(strtolower($books_arr[0]));
        $date = mktime(0, 0, 0, $month, $day, $form_state['values']['year']);
        $o = new stdClass();
        $o->date = $date;
        $o->endbook = $o->startbook = $books[$book]->id;
        if (is_numeric($start_chapter)) {
          $o->startchapter = $start_chapter;
          if (is_numeric($end_chapter)) {
            $o->endchapter = $end_chapter;
          }
        }
        drupal_write_record('biblereadings_readings', $o);
        $count++;
        //$records[] = $o;
      }
    }
  }

  drupal_set_message(t("Successfully Imported !count Bible Readings", array('!count' => $count)));
}

function biblereadings_populate_books() {
  $path = drupal_get_path('module', 'biblereadings');
  $file = fopen("$path/resources/bible_books_pipe.csv", 'r');

  $books = array();

  $count = 0;
  while ($line = trim(fgets($file))) {
    $split = explode('|', $line);
    $o = new stdClass();
    $o->position = $split[0];
    $o->abbreviation = $split[1];
    $o->name = $split[2];
    $o->chapters = $split[3];

    drupal_write_record('biblereadings_books', $o);
    $count++;
  }

  fclose($file);

  drupal_set_message("Successfully populated Bible Readings database with $count books.");
}

function biblereadings_get_books() {
  $sql = "select * from {biblereadings_books} order by position";
  $result = db_query($sql);
  $books = array();
  while ($o = db_fetch_object($result)) {
    $books[strtolower($o->name)] = $o;
  }
  return $books;
}

function biblereadings_schedule_page($year = null) {
  if (!$year) {
    $year = date('Y');
  }
  $start = strtotime('1/1/' . $year);
  $end = strtotime('12/31/' . $year . ' 23:59:59');

  $sql = "SELECT r.*, b.name
      FROM {biblereadings_readings} r
      JOIN {biblereadings_books} b ON b.id = r.startbook
      WHERE r.date between %d and %d
      ORDER BY r.date";
  $result = db_query($sql, $start, $end);

  $header = array(t('Date'), t('Book'), t('Start'), t('End'), '');

  while($o = db_fetch_object($result)) {
    $rows[] = array(
        date('F j, Y', $o->date),
        $o->name,
        $o->startchapter,
        $o->endchapter,
        l(t('edit'), 'admin/settings/biblereadings/' . $o->id . '/edit'),
    );
  }

  $output .= theme('table', $header, $rows);

  return $output;
}

function biblereadings_edit_form(&$form_state, $reading) {
  $form = array();

  $books = array();
  $sql = "select * from {biblereadings_books} order by position";
  $result = db_query($sql);
  while($o = db_fetch_object($result)) {
    $books[$o->id] = $o->name;
  }


  $form['id'] = array(
      '#type' => 'hidden',
      '#value' => $reading->id,
  );

  $form['date'] = array(
      '#type' => 'date',
      '#title' => t('Date'),
      '#default_value' => array('year' => date('Y', $reading->date), 'month' => date('n', $reading->date), 'day' => date('j', $reading->date)),
  );


  $form['startbook'] = array(
      '#type' => 'select',
      '#title' => t('Book'),
      '#options' => $books,
      '#default_value' => $reading->startbook,
  );

  $form['startchapter'] = array(
      '#type' => 'textfield',
      '#title' => t('Start Chapter'),
      '#default_value' => $reading->startchapter,
  );

  $form['endchapter'] = array(
      '#type' => 'textfield',
      '#title' => t('End Chapter'),
      '#default_value' => $reading->endchapter,
  );

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save Changes'));

  return $form;
}

function biblereadings_edit_form_submit($form, &$form_state) {
  $v = $form_state['values'];

  //dpm($v);

  $reading = biblereading_load($v['id']);
  $reading->date = mktime(0, 0, 0, $v['date']['month'], $v['date']['day'], $v['date']['year']);
  $reading->startbook = $v['startbook'];
  $reading->startchapter = $v['startchapter'];
  $reading->endchapter = $v['endchapter'];

  drupal_write_record('biblereadings_readings', $reading, 'id');
  drupal_set_message('Bible reading entry saved.');
  $form_state['redirect'] = 'admin/settings/biblereadings/schedule';
}