<?php

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function theme_biblereadings_readings_this_week(&$vars) {
  $readings = $vars['readings'];
  $show_downloads = $vars['show_downloads'];
  $abbreviated = $vars['abbreviated'];

  $path = drupal_get_path('module', 'biblereadings');
  drupal_add_css("$path/css/biblereadings.css");

  $today = strtotime(date('Y-m-d'));

  $output = '';

  $dt_format = 'l';

  $head = array(t('Day'), t('Devotions'), t('Readings'));
  $rows = array();

  foreach ($readings as $i) {
    $class = (date('Y-m-d', $i->date) == date('Y-m-d', $today)) ? 'today' : '';
    if ($abbreviated) {
      $regular = $i->abbreviation;
    }
    else {
      $regular = $i->bookname;
    }
    if ($i->startchapter) {
      $regular .= sprintf(' %s-%s', $i->startchapter, $i->endchapter);
    }
    if ($i->psalms[1]) {
      if ($abbreviated) {
        $ps = t('Ps');
      }
      else {
        $ps = t('Psalms');
      }

      $psalms = sprintf("%s %d-%d", $ps, $i->psalms[0], $i->psalms[1]);
    }
    else {
      if ($abbreviated) {
        $ps = t('Ps');
      }
      else {
        $ps = t('Psalm');
      }

      $psalms = sprintf("%s %d", $ps, $i->psalms[0]);
    }
    $psalms .= ' / ';
    if ($abbreviated) {
      $pr = t('Pr');
    }
    else {
      $pr = t('Proverbs');
    }
    $psalms .= sprintf('%s %d', $pr, $i->proverb);

    $rows[] = array(
        'class' => array($class),
        'data' => array(
            date($dt_format, $i->date),
            $psalms,
            $regular,
        ),
    );
  }

  $output .= theme('table', array('header' => $head, 'rows' => $rows, 'attributes' => array('id' => 'biblereadings-this-week')));

  if ($show_downloads) {
    $links = array();
    $standard = variable_get('biblereadings_standard_readings_download_path', '');
    $chronological = variable_get('biblereadings_chronological_readings_download_path', '');

    if ($standard) {
      $links[] = l('Standard Bible reading plan', file_create_url($standard));
    }
    if ($chronological) {
      $links[] = l('Chronological Bible reading plan', $chronological, array('attributes' => array('target' => '_blank')));
    }

    $output .= theme('item_list', array('items' => $links, 'title' => 'Download a Printable Reading Plan'));
  }

  return $output;
}
