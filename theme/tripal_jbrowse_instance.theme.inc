<?php

/**
 * Implements hook_preprocess().
 *
 * @param array $variables
 */
function tripal_jbrowse_instance_preprocess(&$variables, $hook) {
  if ($hook !== 'tripal_jbrowse_instance_admin') {
    return;
  }

  $sql = 'SELECT id, common_name, genus, species FROM [tripal_jbrowse_instance] as jbrowse
          JOIN {organism} AS organism ON organism.organism_id=jbrowse.organism_id';

  $results = chado_query($sql);

  $variables['has_records'] = TRUE;

  if ($results->rowCount() < 1) {
    $variables['has_records'] = FALSE;
    return;
  }

  // Create the table
  $header = [
    t('Organism'),
    'Actions' => [
      'data' => t('Actions'),
      'colspan' => 2,
    ],
  ];

  $rows = [];

  while ($record = $results->fetchObject()) {
    $rows[] = [
      t("{$record->common_name} ({$record->genus} {$record->species})"),
      l('Edit', "admin/tripal/extension/jbrowse_instance/edit/{$record->id}"),
      l('Delete', "admin/tripal/extension/jbrowse_instance/delete/{$record->id}", [
        'attributes' => [
          'class' => ['tripal-feature-jbrowse-delete']
        ]
      ]),
    ];
  }

  $variables['list_table'] = theme_table([
    'header' => $header,
    'rows' => $rows,
    'attributes' => [
      'id' => 'tripal_jbrowse_instance-table',
      'class' => ['table-striped'],
    ],
    'sticky' => TRUE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ]);
}