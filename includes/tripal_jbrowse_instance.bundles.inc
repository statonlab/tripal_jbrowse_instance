<?php

/**
 * Create a jbrowse instance bundle
 *
 */

$args = [];
$args['vocabulary'] = 'local';
$args['accession'] = 'jbrowse_instance';
$args['term_name'] = 'jbrowse_instance';
$args['storage_args'] = [
    'data_table' => 'analysis',
];

$term = tripal_load_term_entity(['vocabulary' => 'local', 'accession' => 'jbrowse_instance']);
$bundle = tripal_load_bundle_entity(['term_id' => $term->id]);
if (! $term or ! $bundle) {
    if (! tripal_create_bundle($args, $error)) {
        throw new Exception($error['!message']);
    }
}

