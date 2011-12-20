<?php

function biblereadings_block_this_week() {
  $data = biblereadings_readings_this_week();

  foreach ($data as $i) {
    $i->psalms = biblereadings_get_today_psalms($i->date);
    $i->proverb = biblereadings_get_today_proverb($i->date);
  }

  return theme('biblereadings_readings_this_week', $data);
}