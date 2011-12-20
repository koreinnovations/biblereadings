<?php

function biblereadings_admin_readings_form() {
  $readings = biblereadings_all_readings();

  return $form;
}

function biblereadings_schedule_form($form, $form_state) {
  $year_options = array();
  for ($i = -1; $i < 4; $i++) {
    $y = date('Y') + $i;
    $year_options[$y] = $y;
  }

  $default = ($form_state['year']) ? $form_state['year'] : date('Y');

  $form['year'] = array(
      '#type' => 'select',
      '#title' => t('Filter by Year'),
      '#options' => $year_options,
      '#default_value' => $default,
  );

  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Go'),
  );

  return $form;
}

function biblereadings_schedule_form_submit($form, $form_state) {
  drupal_goto('admin/config/biblereadings/schedule/' . $form_state['values']['year']);
}