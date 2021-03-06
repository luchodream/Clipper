<?php

require_once dirname(__FILE__) . '/file_entity.pages.inc';

/**
 * @file
 * Administrative interface for file type configuration.
 */

/**
 * Administrative form for browsing files and performing operations on them.
 */
function file_entity_admin_files($form, &$form_state) {
  if (!empty($form_state['values']['operation'])) {
    // The form is being rebuilt because an operation requiring confirmation
    // was selected. Hand off to the callback of the operation at this point
    // which should return a confirm form.
    $operation = $form_state['values']['operation'];
    $files = array_keys(array_filter($form_state['values']['files']));
    return call_user_func_array($operation['callback'], array($files));
  }

  $limit = variable_get('file_entity_admin_files_limit', 50);

  // Build the 'Update options' form.

  $options = array();
  $form['#operations'] = file_entity_get_file_operation_info();
  foreach ($form['#operations'] as $operation => $array) {
    $options[$operation] = $array['label'];
  }
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
    '#access' => user_access('administer files') && !empty($options),
  );
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#title_display' => 'invisible',
    '#options' => $options,
  );
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );

  // Build the sortable table header.
  $header = array(
    'title' => array('data' => t('Title'), 'field' => 'f.filename'),
    'type' => array('data' => t('Type'), 'field' => 'f.type'),
    'size' => array('data' => t('Size'), 'field' => 'f.filesize'),
    'author' => array('data' => t('Author'), 'field' => 'u.name'),
    'timestamp' => array('data' => t('Updated'), 'field' => 'f.timestamp', 'sort' => 'desc'),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('file_managed', 'f')->extend('PagerDefault')->extend('TableSort');
  $query->join('users', 'u', 'f.uid = u.uid');
  $query->fields('f', array('fid'));
  $query->fields('u', array('uid'));
  $query->condition('f.status', FILE_STATUS_PERMANENT);
  $query->limit($limit);
  $query->orderByHeader($header);
  $query->addTag('admin_files');
  $query->addMetaData('form_state', $form_state);

  foreach (array_keys(file_entity_get_hidden_stream_wrappers()) as $name) {
    $query->condition('f.uri', $name . '%', 'NOT LIKE');
  }

  // Result array keys are file IDs, values are the file owner's UIDs.
  $result = $query->execute()->fetchAllKeyed();

  // Load all the file entities.
  $files = $form['#files'] = file_load_multiple(array_keys($result));

  // Hide the operations form if there are no files to operate on.
  $form['options']['#access'] &= !empty($files);

  // Load all the file owner user entities to display usernames.
  $accounts = $form['#accounts'] = user_load_multiple(array_unique($result));

  $destination = drupal_get_destination();
  $options = array();
  foreach ($files as $file) {
    $file_type = file_type_load($file->type);
    $options[$file->fid] = array(
      'title' => theme('file_entity_file_link', array('file' => $file)),
      'type' => $file_type ? check_plain($file_type->label) : FILE_TYPE_NONE,
      'size' => format_size($file->filesize),
      'author' => theme('username', array('account' => $accounts[$file->uid])),
      'timestamp' => format_date($file->timestamp, 'short'),
    );

    // Show a warning for files that do not exist.
    if (@!file_exists($file->uri)) {
      $options[$file->fid]['#attributes']['class'][] = 'error';
      if (!file_stream_wrapper_get_instance_by_uri($file->uri)) {
        $options[$file->fid]['#attributes']['title'] = t('The stream wrapper for @scheme files is missing.', array('@scheme' => file_uri_scheme($file->uri)));
      }
      else {
        $options[$file->fid]['#attributes']['title'] = t('The file does not exist.');
      }
    }

    $options[$file->fid]['operations'] = array(
      'data' => array(
        '#theme' => 'links__file_operations',
        '#links' => array(),
        '#attributes' => array('class' => array('links', 'inline')),
      ),
    );
    if (file_entity_access('edit', $file)) {
      $options[$file->fid]['operations']['data']['#links']['edit'] = array(
        'title' => t('Edit'),
        'href' => 'file/' . $file->fid . '/edit',
        'query' => $destination,
      );
    }
    if (file_entity_access('delete', $file)) {
      $options[$file->fid]['operations']['data']['#links']['delete'] = array(
        'title' => t('Delete'),
        'href' => 'file/' . $file->fid . '/delete',
        'query' => $destination,
      );
    }
  }

  $form['files'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No files available.'),
    '#attributes' => array('class' => array('file-display-table', 'clear')),
  );
  $form['pager'] = array('#markup' => theme('pager', array('tags' => NULL)));

  return $form;
}


/**
 * Validate file_entity_admin_files form submissions.
 *
 * Check if any files have been selected to perform the chosen
 * 'Update option' on.
 */
function file_entity_admin_files_validate($form, &$form_state) {
  // Error if there are no items to select.
  if (!is_array($form_state['values']['files']) || !count(array_filter($form_state['values']['files']))) {
    form_set_error('', t('No files selected.'));
  }
}

/**
 * Submit handler for file_entity_admin_files.
 */
function file_entity_admin_files_submit($form, &$form_state) {
  $operations = $form['#operations'];
  $operation = $operations[$form_state['values']['operation']];

  // Filter out unchecked nodes
  $files = &$form_state['values']['files'];
  $files = array_filter($files);

  if (!empty($operation['confirm'])) {
    // In the case of an operation which needs confirmation, rebuild the form.
    $form_state['rebuild'] = TRUE;
    $form_state['values']['operation'] = $operation;
    return;
  }
  elseif ($function = $operation['callback']) {
    // Run the callback without confirmation.
    if (isset($operation['callback arguments'])) {
      $args = array_merge(array($files), $operation['callback arguments']);
    }
    else {
      $args = array($files);
    }
    call_user_func_array($function, $args);
    cache_clear_all();
  }
}

/**
 * Displays the file type admin overview page.
 */
function file_entity_list_types_page() {
  $file_entity_info = entity_get_info('file');
  $field_ui = module_exists('field_ui');
  $header = array(
    array('data' => t('Name')),
    array('data' => t('Operations'), 'colspan' => $field_ui ? '4' : '2'),
  );
  $rows = array();

  foreach (file_type_get_all_types(TRUE) as $type) {
    $row = array(array('data' => theme('file_entity_file_type_overview', array('label' => $type->label, 'description' => $type->description))));
    $path = isset($file_entity_info['bundles'][$type->type]['admin']['real path']) ? $file_entity_info['bundles'][$type->type]['admin']['real path'] : NULL;

    $row[] = array('data' => isset($path) ? l(t('edit file type'), $path . '/edit') : '');
    if ($field_ui) {
      $row[] = array('data' => isset($path) ? l(t('manage fields'), $path . '/fields') : '');
      $row[] = array('data' => isset($path) ? l(t('manage display'), $path . '/display') : '');
    }
    $row[] = array('data' => isset($path) ? l(t('manage file display'), $path . '/file-display') : '');

    $rows[] = $row;
  }

  $build['file_type_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No file types available.'),
  );

  return $build;
}

/**
 * Form callback; presents file display settings for a given view mode.
 */
function file_entity_file_display_form($form, &$form_state, $file_type, $view_mode) {
  $file_type = $file_type->type;

  $form['#file_type'] = $file_type;
  $form['#view_mode'] = $view_mode;
  $form['#tree'] = TRUE;
  $form['#attached']['js'][] = drupal_get_path('module', 'file_entity') . '/file_entity.admin.js';

  // Retrieve available formatters for this file type and load all configured
  // filters for existing text formats.
  $formatters = file_info_formatter_types();
  foreach ($formatters as $name => $formatter) {
    if (isset($formatter['file types']) && !in_array($file_type, $formatter['file types'])) {
      unset($formatters[$name]);
    }
  }
  $current_displays = file_displays_load($file_type, $view_mode, TRUE);
  foreach ($current_displays as $name => $display) {
    $current_displays[$name] = (array) $display;
  }

  // Formatter status.
  $form['displays']['status'] = array(
    '#type' => 'item',
    '#title' => t('Enabled displays'),
    '#prefix' => '<div id="file-displays-status-wrapper">',
    '#suffix' => '</div>',
  );
  $i=0;
  foreach ($formatters as $name => $formatter) {
    $form['displays']['status'][$name] = array(
      '#type' => 'checkbox',
      '#title' => check_plain($formatter['label']),
      '#default_value' => !empty($current_displays[$name]['status']),
      '#description' => isset($formatter['description']) ? filter_xss($formatter['description']) : NULL,
      '#parents' => array('displays', $name, 'status'),
      '#weight' => (isset($formatter['weight']) ? $formatter['weight'] : 0) + ($i / 1000),
    );
    $i++;
  }

  // Formatter order (tabledrag).
  $form['displays']['order'] = array(
    '#type' => 'item',
    '#title' => t('Display precedence order'),
    '#theme' => 'file_entity_file_display_order',
  );
  foreach ($formatters as $name => $formatter) {
    $form['displays']['order'][$name]['label'] = array(
      '#markup' => check_plain($formatter['label']),
    );
    $form['displays']['order'][$name]['weight'] = array(
      '#type' => 'weight',
      '#title' => t('Weight for @title', array('@title' => $formatter['label'])),
      '#title_display' => 'invisible',
      '#delta' => 50,
      '#default_value' => isset($current_displays[$name]['weight']) ? $current_displays[$name]['weight'] : 0,
      '#parents' => array('displays', $name, 'weight'),
    );
    $form['displays']['order'][$name]['#weight'] = $form['displays']['order'][$name]['weight']['#default_value'];
  }

  // Formatter settings.
  $form['display_settings_title'] = array(
    '#type' => 'item',
    '#title' => t('Display settings'),
  );
  $form['display_settings'] = array(
    '#type' => 'vertical_tabs',
  );
  $i=0;
  foreach ($formatters as $name => $formatter) {
    if (isset($formatter['settings callback']) && ($function = $formatter['settings callback']) && function_exists($function)) {
      $defaults = !empty($formatter['default settings']) ? $formatter['default settings'] : array();
      $settings = !empty($current_displays[$name]['settings']) ? $current_displays[$name]['settings'] : array();
      $settings += $defaults;
      $settings_form = $function($form, $form_state, $settings, $name, $file_type, $view_mode);
      if (!empty($settings_form)) {
        $form['displays']['settings'][$name] = array(
          '#type' => 'fieldset',
          '#title' => check_plain($formatter['label']),
          '#parents' => array('displays', $name, 'settings'),
          '#group' => 'display_settings',
          '#weight' => (isset($formatter['weight']) ? $formatter['weight'] : 0) + ($i / 1000),
        ) + $settings_form;
      }
    }
    $i++;
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save configuration'));

  return $form;
}

/**
 * Process file display settings form submissions.
 */
function file_entity_file_display_form_submit($form, &$form_state) {
  $file_type = $form['#file_type'];
  $view_mode = $form['#view_mode'];
  $displays = isset($form_state['values']['displays']) ? $form_state['values']['displays'] : array();
  $displays_original = file_displays_load($file_type, $view_mode, TRUE);
  foreach ($displays as $formatter_name => $display) {
    $display_original = isset($displays_original[$formatter_name]) ? $displays_original[$formatter_name] : file_display_new($file_type, $view_mode, $formatter_name);
    $display += (array) $display_original;
    file_display_save((object) $display);
  }
  drupal_set_message(t('Your settings have been saved.'));
}

/**
 * Returns HTML for a file type label and description for the file type admin overview page.
 */
function theme_file_entity_file_type_overview($variables) {
  return check_plain($variables['label']) . '<div class="description">' . $variables['description'] . '</div>';
}

/**
 * Returns HTML for a file display's display order table.
 */
function theme_file_entity_file_display_order($variables) {
  $element = $variables['element'];

  $rows = array();
  foreach (element_children($element, TRUE) as $name) {
    $element[$name]['weight']['#attributes']['class'][] = 'file-display-order-weight';
    $rows[] = array(
      'data' => array(
        drupal_render($element[$name]['label']),
        drupal_render($element[$name]['weight']),
      ),
      'class' => array('draggable'),
    );
  }
  $output = drupal_render_children($element);
  $output .= theme('table', array('rows' => $rows, 'attributes' => array('id' => 'file-displays-order')));
  drupal_add_tabledrag('file-displays-order', 'order', 'sibling', 'file-display-order-weight', NULL, NULL, TRUE);

  return $output;
}

/**
 * Form constructor for the file type settings form.
 *
 * @param object $type
 *   The file type.
 *
 * @see file_entity_file_type_form_validate()
 * @see file_entity_file_type_form_submit()
 */
function file_entity_file_type_form($form, &$form_state, $type) {
  $form['#file_type'] = $type->type;

  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('This is the human readable name of the file type.'),
    '#required' => TRUE,
    '#default_value' => $type->label,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('This is the description of the file type.'),
    '#default_value' => $type->description,
  );

  $form['mimetypes'] = array(
    '#type' => 'textarea',
    '#title' => t('Mimetypes'),
    '#description' => t('Enter one mimetype per line.'),
    '#default_value' => implode("\n", $type->mimetypes),
  );

  include_once DRUPAL_ROOT . '/includes/file.mimetypes.inc';
  $mimetypes = file_mimetype_mapping();

  $form['mimetype_mapping'] = array(
    '#type' => 'fieldset',
    '#title' => t('Mimetype List'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['mimetype_mapping']['mapping'] = array(
    '#theme' => 'item_list',
    '#items' => $mimetypes['mimetypes'],
  );

  // Options for allowed Streams.
  $options = array('public' => t('Public files'), 'private' => t('Private files'));
  foreach (file_get_stream_wrappers() as $stream => $wrapper) {
    $options[$stream] = $wrapper['name'];
  }
  unset($options['temporary']);
  $default_value = array();
  if (isset($type->streams)) {
    foreach ($type->streams as $stream) {
      $default_value[$stream] = $stream;
    }
  }
  $form['streams'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Allowed streams'),
    '#options' => $options,
    '#default_value' => $default_value,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Form validation handler for file_entity_file_type_form().
 *
 * @see file_entity_file_type_form_submit()
 */
function file_entity_file_type_form_validate($form, &$form_state) {
  include_once DRUPAL_ROOT . '/includes/file.mimetypes.inc';
  $mimetype_mapping = file_mimetype_mapping();

  $valid_mimetypes = $mimetype_mapping['mimetypes'];
  $submitted_mimetypes = array_filter(array_map('trim', explode("\n", $form_state['values']['mimetypes'])));

  $invalid_mimetypes = array_diff($submitted_mimetypes, $valid_mimetypes);
  foreach ($invalid_mimetypes as $mimetype) {
    form_set_error('mimetypes', t('The mimetype %mimetype is not a valid mimetype.', array('%mimetype' => $mimetype)));
  }
}

/**
 * Form submission handler for file_entity_file_type_form().
 *
 * @see file_entity_file_type_form_validate()
 */
function file_entity_file_type_form_submit($form, &$form_state) {
  $type = file_type_load($form['#file_type']);

  $type->label = $form_state['values']['label'];
  $type->description = $form_state['values']['description'];
  $type->mimetypes = array_filter(array_map('trim', explode("\n", $form_state['values']['mimetypes'])));
  $type->streams = array();
  foreach ($form_state['values']['streams'] as $stream) {
    if ($stream) {
      $type->streams[] = $stream;
    }
  }

  file_type_save($type);

  drupal_set_message(t('The file type %type has been updated.', array('%type' => $type->label)));
  $form_state['redirect'] = 'admin/structure/file-types';
}
