<?php

function biblereadings_cron() {

  $now = time();
  $last_sent = variable_get('biblereadings_last_sent', 0);
  $midnight = strtotime(date('m/d/Y'));
  $cutoff_time = $midnight + (3 * 60 * 60); // 3:00 am today
  $ready_to_send = ($last_sent < $cutoff_time);

  if ($ready_to_send == true) {

    // Pull a list of subscribers
    $list = biblereadings_get_subscribers();   

    // Pull today's readings using our API function
    // Loop through list of subscribers and send a message to each
    foreach ($list as $item) {
      // Send message here
      biblereadings_email_daily_readings_to_user(time(), $item);
    }

    variable_set('biblereadings_last_sent', time());
  }
}

function biblereadings_email_daily_readings_to_user($date, $user) {
  static $readings;
  static $default_params;

  if (!isset($readings)) {
    $readings = biblereadings_readings_by_date($date);
  }

  if (!isset($default_params)) {
    $default_params = array(
        'subject' => t('Your Bible Readings for !date', array('!date' => date('F j'))),
        'readings' => $readings,
    );
  }

  $params = array();
  $params['name'] = $user['name'];
  $params += $default_params;

  // D7 converts html mail to plain text.  In the interest of time, we are going to flow
  // with this instead of building an alternate implementation of the mail system.
  // See here for details when ready:
  // http://drupal.org/node/900794
//  if ($user['preference'] == 'html') {
//    $mail_key = 'biblereadings_html_mail';
//  }
//  else {
  $mail_key = 'biblereadings_normal_mail';
//  }
  drupal_mail('biblereadings', $mail_key, $user['email'], language_default(), $params);
}

/*
 * Implementation of hook_mail()
 */

function biblereadings_mail($key, &$message, $params) {


  /*
   * This function is used to hook into drupal_mail function to send the mail
   * Everytime drupal_mail is used, this function is called to setup the mail to be sent
   */
  $language = $message['language'];
  switch ($key) {
    /*
     * Here we will be switching the key values, therefore different mails can be sent differently using different keys.
     * As demonstrated above, we have different keys for the admin and other users to send normal mail to admins, and html mails to users
     */
    case 'biblereadings_normal_mail':
      /* Emails with this key will be normal emails, so we just set body and subject accordingly */
      $message['subject'] = $params['subject'];
      //the email body is here, inside the $message array
      $message['body'][] = theme('biblereadings_daily_email_text', array('name' => $params['name'], 'date' => time(), 'readings' => $params['readings']));
      break;
    case 'biblereadings_html_mail':
      $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';

      /* This refers to line 20 and calls the subject */
      $message['subject'] = $params['subject'];
      /* the email body is here, inside the $message array */
      $message['body'][] = theme('biblereadings_daily_email_html', array('name' => $params['name'], 'date' => time(), 'readings' => $params['readings']));
      break;
  }
}

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

  $items['biblereadings/unsubscribe'] = array(
      'title' => 'Unsubscribe from Daily Bible Readings',
      'page callback' => 'biblereadings_page_unsubscribe',
      'access callback' => TRUE,
      'file' => 'includes/pages.inc',
  );

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
      'file' => 'includes/admin.inc',
  );

  $items['admin/config/biblereadings/schedule'] = array(
      'title' => 'Schedule',
      'page callback' => 'biblereadings_schedule_page',
      'access arguments' => array('administer bible readings'),
      'type' => MENU_LOCAL_TASK,
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

  $items['biblereadings_daily_email_html'] = array(
      'file' => 'includes/theme.inc',
      'variables' => array(
          'name' => NULL,
          'date' => NULL,
          'readings' => array(),
      ),
      'template' => 'templates/biblereadings_daily_email_html',
  );

  $items['biblereadings_daily_email_text'] = array(
      'file' => 'includes/theme.inc',
      'variables' => array(
          'name' => NULL,
          'date' => NULL,
          'readings' => array(),
      ),
      'template' => 'templates/biblereadings_daily_email_text',
  );

  return $items;
}

function biblereadings_block_info() {
  $blocks = array();
  $blocks['weekly_readings']['info'] = t('Weekly Bible Readings');
  $blocks['subscription']['info'] = t('Bible Readings Subscription Form');
  return $blocks;
}

function biblereadings_block_view($delta = '') {
  $block = array();
  switch ($delta) {
    case 'weekly_readings':
      $readings = biblereadings_readings_this_week();
      $block['content'] = theme('biblereadings_readings_this_week', array('readings' => $readings, 'show_downloads' => TRUE));
      break;
    case 'subscription':
      $form = drupal_get_form('biblereadings_subscribe_form');
      $block['content'] = drupal_render($form);
      break;
  }
  return $block;
}

function biblereadings_ctools_plugin_directory($module, $plugin) {
  if ($module == 'webmobilize' || $module == 'ctools') {
    return 'plugins/' . $plugin;
  }
}


/**
 * Implementation of hook_views_api().
 */
function biblereadings_views_api() {
  return array(
      'api' => 2,
      'path' => drupal_get_path('module', 'biblereadings') . '/includes/views',
  );
}