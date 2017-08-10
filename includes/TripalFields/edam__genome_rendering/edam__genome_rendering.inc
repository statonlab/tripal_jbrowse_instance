<?php

/**
 * @class
 * Purpose:  This field links out to a JBrowse instance.
 * Data:
 * Assumptions:
 */
class edam__genome_rendering extends ChadoField
{
    // --------------------------------------------------------------------------
    //                     EDITABLE STATIC CONSTANTS
    //
    // The following constants SHOULD be set for each descendant class.  They are
    // used by the static functions to provide information to Drupal about
    // the field and it's default widget and formatter.
    // --------------------------------------------------------------------------

    // The default label for this field.
    public static $default_label = 'JBrowse Instance';

    // The default description for this field.
    public static $default_description = 'Link to JBrowse location';

    // The default widget for this field.
    public static $default_widget = 'edam__genome_rendering_widget';

    // The default formatter for this field.
    public static $default_formatter = 'edam__genome_rendering_formatter';

    // The module that manages this field.
    public static $module = 'tripal_jbrowse_instance';

    // A list of global settings. These can be accessed within the
    // globalSettingsForm.  When the globalSettingsForm is submitted then
    // Drupal will automatically change these settings for all fields.
    // Once instances exist for a field type then these settings cannot be
    // changed.
    public static $default_settings = [
        'storage' => 'field_chado_storage',
        // It is expected that all fields set a 'value' in the load() function.
        // In many cases, the value may be an associative array of key/value pairs.
        // In order for Tripal to provide context for all data, the keys should
        // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
        // function that are supported by the query() function should be
        // listed here.
        'searchable_keys' => [],
    ];

    /**
     * @param $bundle_name
     *   Machine name of bundle, example bio_data_1
     * @param $fields
     *Array of the machine names of the children of the field group being created.
     * Set up pane group
     */

    // in ds, this gets called ds_layout_settings_info, which is called in tripal_ds_bundle_postcreate($bundle)

    //here we call it the Tripal hook_bundle_postcreate


    function _tripal_jbrowse_instance_field_group_info($bundle_name, $fields)
    {
        //Tripal pane to nest the fieldset within.
        $field_group_tripalpane = new stdClass();
        $field_group_tripalpane->disabled = false; /* Edit this to true to make a default field_group disabled initially*/
        $field_group_tripalpane->api_version = 1;
        $field_group_tripalpane->identifier = 'genome_rendering_tripalpane|TripalEntity|'.$bundle_name.'|default';
        $field_group_tripalpane->group_name = 'genome_rendering_tripalpane';
        $field_group_tripalpane->entity_type = 'TripalEntity';
        $field_group_tripalpane->bundle = $bundle_name;
        $field_group_tripalpane->mode = 'default';
        $field_group_tripalpane->parent_name = '';
        $field_group_tripalpane->data = [
            'label' => 'Data Sequences',
            'weight' => '1',
            'children' => [
                0 => 'genome_rendering_table',
            ],
            'format_type' => 'tripalpane',
            'format_settings' => [
                'label' => 'JBROWSE',
                'instance_settings' => [
                    'id' => 'tripal_jbrowse_instance_tripalpane',
                    //'classes' => 'group-sequence-tripalpane field-group-tripalpane ',
                    'edam__genome_rendering',
                    'description' => '',
                    'hide' => 1,
                ],
            ],
        ];
        drupal_write_record('field_group', $field_group_tripalpane);
        //Write to the tripal_ds table to record the new tripal pane.
        tripal_ds_bundle_menu_item($bundle_name, 'Genome Rendering', 'genome_rendering_tripalpane', 'TripalEntity');

        ////Table of fields.
        //$field_group = new stdClass();
        //$field_group->disabled = false; /* Edit this to true to make a default field_group disabled initially*/
        //$field_group->api_version = 1;
        //$field_group->identifier = 'group_sequence_table|TripalEntity|'.$bundle_name.'|default';
        //$field_group->group_name = 'group_sequence_table';
        //$field_group->entity_type = 'TripalEntity';
        //$field_group->bundle = $bundle_name;
        //$field_group->mode = 'default';
        //$field_group->parent_name = 'group_sequence_tripalpane';
        //$field_group->data = [
        //    'label' => 'Data Sequences Table',
        //    'weight' => '1',
        //    'children' => $fields,
        //    'format_type' => 'table',
        //    'format_settings' => [
        //        'label' => 'Data Sequences Table',
        //        'instance_settings' => [
        //            'label_visibility' => '1',
        //            'desc' => '',
        //            'first_column' => '',
        //            'second_column' => '',
        //            'empty_label_behavior' => '1',
        //            'table_row_striping' => 0,
        //            'always_show_field_label' => 0,
        //            'classes' => 'group-sequence-table field-group-table',
        //        ],
        //    ],
        //];
        //drupal_write_record('field_group', $field_group);
    }


// Provide a list of instance specific settings. These can be access within
    // the instanceSettingsForm.  When the instanceSettingsForm is submitted
    // then Drupal with automatically change these settings for the instance.
    // It is recommended to put settings at the instance level whenever possible.
    // If you override this variable in a child class be sure to replicate the
    // term_name, term_vocab, term_accession and term_fixed keys as these are
    // required for all TripalFields.
    public static $default_instance_settings = [
        // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
        'term_vocabulary' => 'edam',
        // The name of the term.
        'term_name' => 'genome_rendering',
        // The unique ID (i.e. accession) of the term.
        'term_accession' => 'genome_rendering',
        // Set to TRUE if the site admin is not allowed to change the term
        // type, otherwise the admin can change the term mapped to a field.
        'term_fixed' => false,
        // Indicates if this field should be automatically attached to display
        // or web services or if this field should be loaded separately. This
        // is convenient for speed.  Fields that are slow should for loading
        // should have auto_attach set to FALSE so tha their values can be
        // attached asynchronously.
        'auto_attach' => false,
        // The table in Chado that the instance maps to.
        'chado_table' => 'organism',
        // The column of the table in Chado where the value of the field comes from.
        'chado_column' => 'organism_id',
        // The base table.
        'base_table' => '',
    ];

    // A boolean specifying that users should not be allowed to create
    // fields and instances of this field type through the UI. Such
    // fields can only be created programmatically with field_create_field()
    // and field_create_instance().
    public static $no_ui = false;

    // A boolean specifying that the field will not contain any data. This
    // should exclude the field from web services or downloads.  An example
    // could be a quick search field that appears on the page that redirects
    // the user but otherwise provides no data.
    public static $no_data = false;

    /**
     * Loads the field values from the underlying data store.
     *
     * @param $entity
     *
     * @return
     *   An array of the following format:
     *     $entity->{$field_name}['und'][0]['value'] = $value;
     *   where:
     *     - $entity is the entity object to which this field is attached.
     *     - $field_name is the name of this field
     *     - 'und' is the language code (in this case 'und' == undefined)
     *     - 0 is the cardinality.  Increment by 1 when more than one item is
     *       available.
     *     - 'value' is the key indicating the value of this field. It should
     *       always be set.  The value of the 'value' key will be the contents
     *       used for web services and for downloadable content.  The value
     *       should be of the follow format types: 1) A single value (text,
     *       numeric, etc.) 2) An array of key value pair. 3) If multiple entries
     *       then cardinality should incremented and format types 1 and 2 should
     *       be used for each item.
     *   The array may contain as many other keys at the same level as 'value'
     *   but those keys are for internal field use and are not considered the
     *   value of the field.
     */
    public function load($entity)
    {
        $record = $entity->chado_record;
        $organism = $record->organism_id;
        $feature_id = $record->feature_id;
        dpm($organism);
        dpm($feature_id);
        $linkout = "fake linkout";

        //  $entity->$linkout = $linkout;
        return $linkout;

        // ChadoFields automatically load the chado column specified in the
        // default settings above. If that is all you need then you don't even
        // need to implement this function. However, if you need to add any
        // additional data to be used in the display, you should add it here.
    }
}
