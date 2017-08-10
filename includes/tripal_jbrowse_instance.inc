<?php

/**
 * Builds the tripal_jbrowse layout.
 *
 * @param $bundle_name
 *  Machine name of bundle, example bio_data_1
 * @param $instances
 *
 * @return bool
 */
function _tripal_jbrowse_instance_layout_settings_info($bundle_name, $instances)
{
    $region_right = [];
    $region_left = [];
    $prop_fields = [];
    $summary_fields = []; //only rdfs__type
    $data_sequence_fields = [];
    $all_other_fields = [];
    $fields_with_regions = [];
    $i = 0;
    $all_fields = [];

    watchdog('debug', '<pre>$bundle_name: '.print_r($bundle_name, true).'</pre>');
    watchdog('debug', '<pre>$instances: '.print_r($instances, true).'</pre>');

    try {
// Get the bundle and term objects.
        $bundle = tripal_load_bundle_entity(['name' => $bundle_name]);
        $term = tripal_load_term_entity(['term_id' => $bundle->term_id]);

// Build one large multidimensional array of all instances
        foreach ($instances as $key => $instance) {
            $all_fields[$i] = $instance;
            $i++;
        }

        // sort in alpha order here
//usort($all_fields, tripal_ds_sort_array('label'));

// Iterate through the fields of this bundle
        foreach ($all_fields as $key => $instance) {
            $instance_name = $instance['field_name'];
            if ($instance_name == "rdfs__type") {
                //rdf:type is a property used to state that a resource is an instance of a class. A commonly accepted qname for this property is "a".[

                array_push($summary_fields, $instance_name);
            } else {
//TODO: How do we handle non-chado dbs, placement of fields within
// tripal panes might need to be done in a hook.
                $instance_base_table = array_key_exists('base_table', $instance['settings']) ? $instance['settings']['base_table'] : '';
                $instance_base_chado = array_key_exists('chado_table', $instance['settings']) ? $instance['settings']['chado_table'] : '';

                // search if prop is in prop table and data__sequence is in instance name.
                $prop_table = strpos($instance_base_chado, 'prop');
                $data_sequence = strpos($instance_name, 'data__sequence');
                if ($instance_base_chado && $instance_base_table) {
                    if ($instance_base_chado == $instance_base_table) {
                        if ($prop_table !== false) {
                            array_push($prop_fields, $instance_name);
                        } elseif ($data_sequence !== false) {
                            array_push($data_sequence_fields, $instance_name);
                        } else {
                            array_push($summary_fields, $instance_name);
                        }
                    } elseif ($instance_base_chado != $instance_base_table) {
                        if ($prop_table !== false) {
                            array_push($prop_fields, $instance_name);
                        } elseif ($data_sequence !== false) {
                            array_push($data_sequence_fields, $instance_name);
                        } else {
                            array_push($all_other_fields, $instance);

// Update the display settings so that the title is hidden.
                            $instance['display']['default']['label'] = 'hidden';
                            field_update_instance($instance);
                        }
                    }
                } else {
// The tripal_chado module adds an image to the organism content
// type so we want to make sure that image goes in the summary.
// It is not a TripalField so it won't have a chado table.
                    if ($instance_name == 'data__image' and $term->name == 'organism') {
                        array_push($summary_fields, $instance_name);
                    }
                }
            }
        }

// Consolidate the field sets.
        if (! empty($summary_fields)) {
            _summary_field_group_info($bundle_name, $summary_fields);
        }
        if (! empty($prop_fields)) {
            _prop_field_group_info($bundle_name, $prop_fields);
        }
        if (! empty($data_sequence_fields)) {
            _data_sequence_field_group_info($bundle_name, $data_sequence_fields);
        }
        if (! empty($all_other_fields)) {
            foreach ($all_other_fields as $key => $other_field) {
                $group_field_name = 'gp_'.$other_field['field_name'];

// Need to truncate the names because of database field size restrictions,
// updating fields here to ensure name consistency.
                $group_field_name = substr($group_field_name, 0, 27);

// Add random numbers to ensure the field name is unique within the 32
// character limit of the field.
                $group_field_name = $group_field_name.rand(0, 99999);
                _additional_fields_field_group_info($bundle_name, $other_field['label'], $group_field_name, $other_field['field_name']);
            }
        }

// Build one large multidimensional array of all instances to sort in alpha
// order to display fields in label alpha order.
        $right_fields = [];
        $all_field_groups = field_group_info_groups('TripalEntity', $bundle_name);

        if (! empty($all_field_groups)) {
            if (is_array($all_field_groups)) {
                if (! isset($all_field_groups['default'])) {
                    $all_field_groups['default'] = [];
                }
                foreach ($all_field_groups['default'] as $key => $field_name) {
                    $right_fields[$key] = $field_name;
                }
                usort($right_fields, tripal_ds_sort_object('label'));
            }
        } elseif (empty($all_field_groups)) {
//Add the original instances that were passed and the field_groups that
//were created.
            $field_group_fields = db_select('field_group', 'fg')->fields('fg', [
                'group_name',
                'data',
            ])->condition('bundle', $bundle_name, '=')->execute()->fetchAll();

            $instance_names = [];
            $field_group_names = [];
            foreach ($all_fields as $key => $instance) {
                $instance_names[$key]['field_name'] = $instance['field_name'];
                $instance_names[$key]['label'] = $instance['label'];
            }
            foreach ($field_group_fields as $key => $field_group_name) {
                $data = unserialize($field_group_name->data);
                $field_group_names[$key]['field_name'] = $field_group_name->group_name;
                $field_group_names[$key]['label'] = $data['format_settings']['label'];
            }
            $all_field_groups = array_merge($instance_names, $field_group_names);
            usort($all_field_groups, tripal_ds_sort_array('label'));
        }

// Now build the $region_right array and the fields array.
        $i = 0;
        if (empty($right_fields)) {
            foreach ($all_field_groups as $index => $field) {
                $region_right[$i] = $field['field_name'];
                $i++;
                tripal_ds_field_group_update_weight($field['field_name'], $bundle_name, $i);
            }
        } elseif (! empty($right_fields)) {
            foreach ($right_fields as $index => $field) {
// Check if the child is already present which is a problem when groups
// are nested within groups.
                if (in_array($field->group_name, $region_right)) {
// Still need to check for children and add them.
                    if (! empty($field->children)) {
                        foreach ($field->children as $index => $child) {
                            $region_right[$i] = $child;
                            $i++;
                        }
                    }
                } else {
                    $region_right[$i] = $field->group_name;
                    if (! empty($field->children)) {
                        foreach ($field->children as $index => $child) {
                            $i++;
                            $region_right[$i] = $child;
                        }
                    }
                    $i++;
                }
// Now update the weights of the field_groups.
                tripal_ds_field_group_update_weight($field->group_name, $bundle_name, $i);
            }
        }
        foreach ($region_right as $index => $field) {
            $fields_with_regions[$field] = 'right';
        }
// Add blocks to $region_left and build the toc field that is placed within.
        _ds_fields_info_write($bundle_name);
        $region_left = ['toc'];
        $fields_with_regions += ['toc' => 'left'];

// Build the ds layout.
        $record = new stdClass;
        $record->id = 'TripalEntity|'.$bundle_name.'|default';
        $record->entity_type = 'TripalEntity';
        $record->bundle = $bundle_name;
        $record->view_mode = 'default';
        $record->layout = 'tripal_ds_feature';
        $settings = [
            'regions' => [
                'left' => $region_left,
                'right' => $region_right,
            ],
            'fields' => $fields_with_regions,
            'classes' => [],
            'wrappers' => [
                'left' => 'div',
                'right' => 'div',
            ],
            'layout_wrapper' => 'div',
            'layout_attributes' => '',
            'layout_attributes_merge' => 1,
            'layout_link_attribute' => '',
            'layout_link_custom' => '',
            'layout_disable_css' => 0,
        ];
        $record->settings = $settings;

        drupal_write_record('ds_layout_settings', $record);
// Clear the Drupal cache.
        drupal_flush_all_caches();
    } catch (Exception $e) {
        watchdog_exception('tripal_ds', $e);

        return false;
    }

    return true;
}
