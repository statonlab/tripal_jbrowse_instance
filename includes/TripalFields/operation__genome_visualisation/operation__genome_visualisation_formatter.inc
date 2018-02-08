<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class operation__genome_visualisation_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'JBrowse Instance Formatter';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['operation__genome_visualisation'];

  // The list of default settings for this formatter.
  public static $default_settings = [
    'key' => 'default_value',
  ];

  /**
   * Provides the field's setting form.
   *
   * This function corresponds to the hook_field_formatter_settings_form()
   * function of the Drupal Field API.
   *
   * The settings form appears on the 'Manage Display' page of the content
   * type administration page. This function provides the form that will
   * appear on that page.
   *
   * To add a validate function, please create a static function in the
   * implementing class, and indicate that this function should be used
   * in the form array that is returned by this function.
   *
   * This form will not be displayed if the formatter_settings_summary()
   * function does not return anything.
   *
   * param $field
   *   The field structure being configured.
   * param $instance
   *   The instance structure being configured.
   * param $view_mode
   *   The view mode being configured.
   * param $form
   *   The (entire) configuration form array, which will usually have no use
   *   here.  Typically for reference only.
   * param $form_state
   *   The form state of the (entire) configuration form.
   *
   * @return
   *   A Drupal Form array containing the settings form for this field.
   */
  public function settingsForm($view_mode, $form, &$form_state) {
  }

  /**
   *  Provides the display for a field
   *
   * This function corresponds to the hook_field_formatter_view()
   * function of the Drupal Field API.
   *
   *  This function provides the display for a field when it is viewed on
   *  the web page.  The content returned by the formatter should only include
   *  what is present in the $items[$delta]['values] array. This way, the
   *  contents that are displayed on the page, via webservices and downloaded
   *  into a CSV file will always be identical.  The view need not show all
   *  of the data in the 'values' array.
   *
   * @param $element
   * @param $entity_type
   * @param $entity
   * @param $langcode
   * @param $items
   * @param $display
   *
   * @return
   *    An element array compatible with that returned by the
   *    hook_field_formatter_view() function.
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $field = $this->field['field_name'];
    $empty_message = '<p style="margin: 10px 0;">A JBrowse instance does not exist for this organism.</p>';
    $exists = $entity->{$field}['und'][0]['value']['instance'];

    if (!$exists) {
      return;
    }

    $element[] = [
      '#type' => 'markup',
      '#markup' => $exists ? $this->render($entity) : $empty_message,
    ];
  }

  /**
   * Provides a summary of the formatter settings.
   *
   * This function corresponds to the hook_field_formatter_settings_summary()
   * function of the Drupal Field API.
   *
   * On the 'Manage Display' page of the content type administration page,
   * fields are allowed to provide a settings form.  This settings form can
   * be used to allow the site admin to define how the field should be
   * formatted.  The settings are then available for the formatter()
   * function of this class.  This function provides a text-based description
   * of the settings for the site developer to see.  It appears on the manage
   * display page inline with the field.  A field must always return a
   * value in this function if the settings form gear button is to appear.
   *
   * See the hook_field_formatter_settings_summary() function for more
   * information.
   *
   * @param $field
   * @param $instance
   * @param $view_mode
   *
   * @return string
   *   A string that provides a very brief summary of the field settings
   *   to the user.
   *
   */
  public function settingsSummary($view_mode) {
  }

  /**
   * Render the pane content.
   *
   * @param $record
   *
   * @return String An HTML string representing the themed output.
   */
  protected function render_template($record) {
    return theme('tripal_jbrowse_instance_pane', ['record' => $record]);
  }

  /**
   * Parse jbrowse links to include tracks.
   *
   * @param $record
   * @param $node
   */
  protected function parse_link(&$record, $entity) {
    $type = $entity->chado_table;
    // Add loc parameters
    if (!empty($record->tracks)) {
      $tracks = json_decode($record->tracks);
      foreach ($tracks as $key => $track) {
        $tracks[$key] = trim($track);
      }

      $tracks = implode(',', $tracks);

      if (strpos($record->link, '?') !== FALSE) {
        $record->link .= "&tracks={$tracks}";
      }
      else {
        $record->link .= "?tracks={$tracks}";
      }
    }

    if ($type === 'feature') {
      $feature_name = $entity->chado_record->uniquename;

      if ($record->regexp) {
        preg_match($record->regexp, $entity->chado_record->uniquename, $matches);
        if (isset($matches[1])) {
          $feature_name = $matches[1];
        }
      }

      $record->link .= "&loc={$feature_name}";
      //$record->link .= "&highlight={$node->feature->uniquename}";
    }
    elseif (!empty($record->default_location)) {
      $record->link .= "&loc={$record->default_location}";
    }
  }

  /**
   * Implements hook_node_view().
   *
   * @param $node
   * @param $view_mode
   * @param $langcode
   *
   * @return mixed|null
   */
  protected function render($entity) {
    $field = $this->field['field_name'];
    $record = $entity->{$field}['und'][0]['value']['instance'];

    $type = $entity->chado_table;

    if ($record) {
      if ($type === 'organism' && !$record->show_in_organism_page) {
        return;
      }
      $this->parse_link($record, $entity);
      return $this->render_template($record);
    }
  }
}
