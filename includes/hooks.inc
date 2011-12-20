<?php

function biblereadings_permission() {
  $items = array();

  $items['administer bible readings'] = array(
      'title' => 'administer bible readings',
      'description' => t('Allows the user to administer bible readings settings'),
  );

  return $items;
}

function biblereadings_menu() {
  $items = array();


  $items['admin/biblereadings/readings'] = array(
      'title' => 'Bible Readings',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('biblereadings_admin_readings_form'),
  );

  $items['admin/config/biblereadings'] = array(
      'title' => 'Bible Readings',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('biblereadings_admin_settings_form'),
      'access arguments' => array('administer bible readings'),
  );

  $items['admin/config/biblereadings/schedule'] = array(
      'title' => 'Schedule',
      'page callback' => 'biblereadings_schedule_page',
      'access arguments' => array('administer bible readings'),
  );

  $items['admin/config/biblereadings/%biblereading/edit'] = array(
      'title' => 'Edit Bible Reading',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('biblereadings_edit_form', 3),
      'access arguments' => array('administer bible readings'),
  );

  $items['admin/config/biblereadings/general'] = array(
      'title' => 'General Settings',
      'type' => MENU_DEFAULT_LOCAL_TASK,
  );

  $items['admin/config/biblereadings/import'] = array(
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

function biblereadings_block_info() {
  $blocks = array();
  $blocks['weekly_readings']['info'] = t('Weekly Bible Readings');
  return $blocks;
}

function biblereadings_block_view($delta = '') {
  $block = array();
  switch ($delta) {
    case 'weekly_readings':
      $readings = biblereadings_readings_this_week();
      $block['content'] = theme('biblereadings_readings_this_week', array('readings' => $readings, 'show_downloads' => TRUE));
      break;
  }
  return $block;
}

function biblereadings_ctools_plugin_directory($module, $plugin) {
  if ($module == 'webmobilize' || $module == 'ctools') {
    return 'plugins/' . $plugin;
  }
}
