<?php

/**
 * @file
 * Administrative page callbacks for the modal_forms module.
 */

/**
 * General configuration form for controlling the modal_forms behaviour.
 */
function modal_forms_admin_settings() {

  // Login settings.
  $form['modal_forms_login_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Login links settings'),
  );
  $form['modal_forms_login_settings']['modal_forms_login'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable for login links'),
    '#default_value' => variable_get('modal_forms_login', 0),
    '#description' => t('Automatically activate Modal forms for links to user/login.'),
  );
  $form['modal_forms_login_settings']['modal_forms_login_links'] = array(
    '#type' => 'radios',
    '#title' => t('Display links'),
    '#options' => array(
      0 => t('No links'),
      1 => t('Show links'),
      2 => t('Show links and open them in a modal'),
    ),
    '#default_value' => variable_get('modal_forms_login_links', 0),
    '#description' => t('Display the "Create new account" (if allowed) and "Request new password" links below the login form.'),
    '#states' => array(
      'visible' => array(
        ':input[name="modal_forms_login"]' => array('checked' => TRUE),
      ),
    ),
  );

  // Register settings.
  $form['modal_forms_register_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Register links settings'),
  );
  $form['modal_forms_register_settings']['modal_forms_register'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable for register new account links'),
    '#default_value' => variable_get('modal_forms_register', 0),
    '#description' => t('Automatically activate Modal forms for links to user/register.'),
  );

  // Password settings.
  $form['modal_forms_password_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Password links settings'),
  );
  $form['modal_forms_password_settings']['modal_forms_password'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable for request new password links'),
    '#default_value' => variable_get('modal_forms_password', 0),
    '#description' => t('Automatically activate Modal forms for links to user/password.'),
  );

  // Comments settings.
  $form['modal_forms_comment_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Comment links settings'),
  );
  $form['modal_forms_comment_settings']['modal_forms_comment'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable for comment links'),
    '#default_value' => variable_get('modal_forms_comment', 0),
    '#description' => t('Automatically activate Modal forms for links to comment.'),
  );

  // Contact settings.
  $form['modal_forms_contact_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Contact links settings'),
  );
  $form['modal_forms_contact_settings']['modal_forms_contact'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable for contact links'),
    '#default_value' => variable_get('modal_forms_contact', 0),
    '#description' => t('Automatically activate Modal forms for links to contact.'),
  );

  // Advanced settings.
  $form['modal_forms_advanced_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['modal_forms_advanced_settings']['modal_forms_visibility'] = array(
    '#type' => 'radios',
    '#title' => t('Show Modal forms on specific pages'),
    '#options' => array(0 => t('All pages except those listed'), 1 => t('Only the listed pages')),
    '#default_value' => variable_get('modal_forms_visibility', 0),
  );
  $form['modal_forms_advanced_settings']['modal_forms_pages'] = array(
    '#type' => 'textarea',
    '#title' => '<span class="element-invisible">' . t('Pages') . '</span>',
    '#default_value' => variable_get('modal_forms_pages', "admin*\nimagebrowser*\nimg_assist*\nimce*\nnode/add/*\nnode/*/edit\nprint/*\nprintpdf/*\nsystem/ajax\nsystem/ajax/*"),
    '#description' => t("Specify pages by using their paths. Enter one path per line. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>')),
  );

  return system_settings_form($form);
}
