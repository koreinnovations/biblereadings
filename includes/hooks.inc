<?php

function biblereadings_perm() {
  $items = array();

  $items[] = 'administer bible readings';

  return $items;
}

function biblereadings_menu() {
  $items = array();


  $items['admin/biblereadings/readings'] = array(
      'title' => 'Bible Readings',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('biblereadings_admin_readings_form'),
  );

  $items['admin/settings/biblereadings'] = array(
      'title' => 'Bible Readings',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('biblereadings_admin_settings_form'),
      'access arguments' => array('administer bible readings'),
  );

  $items['admin/settings/biblereadings/schedule'] = array(
      'title' => 'Schedule',
      'page callback' => 'biblereadings_schedule_page',
      'access arguments' => array('administer bible readings'),
  );

  $items['admin/settings/biblereadings/%biblereading/edit'] = array(
      'title' => 'Edit Bible Reading',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('biblereadings_edit_form', 3),
      'access arguments' => array('administer bible readings'),
  );

  $items['admin/settings/biblereadings/general'] = array(
      'title' => 'General Settings',
      'type' => MENU_DEFAULT_LOCAL_TASK,
  );

  $items['admin/settings/biblereadings/import'] = array(
      'title' => 'Import Reading Plan',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('biblereadings_admin_import_text_form'),
      'access arguments' => array('administer bible readings'),
      'type' => MENU_LOCAL_TASK,
  );

  return $items;
}

function biblereadings_theme() {
  $items = array();

  $items['biblereadings_readings_this_week'] = array(
      'arguments' => array('readings' => array(), 'show_downloads' => true, 'abbreviated' => false),
  );

  return $items;
}

function biblereadings_block($op = 'list', $delta = 0, $edit = array()) {
  switch ($op) {
    case 'list':
      $blocks = array();
      $blocks[0]['info'] = 'Weekly Bible Readings';
      return $blocks;

    case('view'):
      $block = array();
      switch ($delta) {
        case 0:
          $block['content'] = biblereadings_block_this_week();
          break;
      }
      return $block;
  }
}

function biblereadings_ctools_plugin_directory($module, $plugin) {
  if ($module == 'webmobilize' || $module == 'ctools') {
    return 'plugins/' . $plugin;
  }
}
