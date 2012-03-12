<?php
function biblereadings_cron() {

  $now =  time();
  $last_sent = variable_get('biblereadings_last_sent',0);
  $next = $last_sent + 60*60*24;


  $one_day = 60;
  $ready_to_send = ($now - $last_sent >= $one_day);
  $to = 'connor.davis@koremedia.net';


  if ($ready_to_send == true){
  }

 $params = array(
        'subject' => t('Test Email Subject'),
        'body' => t("Body of the email for test"),
      );
      /* If you notice, we use different keys for the admin and other users, so in hook_mail, we can adjust the admin emails a bit if needed */
      drupal_mail("biblereadings", "biblereadings_normal_mail", $to, language_default(), $params, "connor.davis94@yahoo.com");
      drupal_mail("biblereadings", "biblereadings_html_mail", $to, language_default(), $params, "connor.davis94@yahoo.com");
}
  /*
   * Implementation of hook_mail()
   */

  function biblereadings_mail($key, $message, $params)
  {
      /*
       * This function is used to hook into drupal_mail function to send the mail
       * Everytime drupal_mail is used, this function is called to setup the mail to be sent
       */
      $language = $message['language'];
      switch ($key)
      {
          /*
           * Here we will be switching the key values, therefore different mails can be sent differently using different keys.
           * As demonstrated above, we have different keys for the admin and other users to send normal mail to admins, and html mails to users
           */
          case 'biblereadings_normal_mail':
              /* Emails with this key will be normal emails, so we just set body and subject accordingly */
              $message['subject'] = t($params['subject'], $var, $language->language);
              //the email body is here, inside the $message array
              $message['body'][] = $params['body'];
              break;
          case 'biblereadings_html_mail':
              /*
               * Emails with this key will be HTML emails,
               * we therefore cannot use drupal default headers, but set our own headers
               */
              /*
               * $vars required even if not used to get $language in there since t takes in: t($string, $args = array(), $langcode = NULL)
               */
              $message['subject'] = t($params['subject'], $var, $language->language);
              /* the email body is here, inside the $message array */
              $body = "<html><body>
                  <h2>HTML Email Sample with Drupal</h2>
                  <hr /><br /><br />
                  {$params['body']}
                  </body></html>";
              $message['body'][] = $body;
              $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
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
