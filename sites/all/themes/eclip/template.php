<?php

/**
 * Override of theme_breadcrumb().
 */
function eclip_breadcrumb($variables) {
  $breadcrumb = $variables['breadcrumb'];
  if (!empty($breadcrumb)) {
    // Provide a navigational heading to give context for breadcrumb links to
    // screen-reader users. Make the heading invisible with .element-invisible.
    $output = '<h2 class="element-invisible">' . t('You are here') . '</h2>';

    $output .= '<div class="breadcrumb">' . implode(' › ', $breadcrumb) . '</div>';
    return $output;
  }
}

/**
 * Override or insert variables into the maintenance page template.
 */
function eclip_preprocess_maintenance_page(&$vars) {
  // While markup for normal pages is split into page.tpl.php and html.tpl.php,
  // the markup for the maintenance page is all in the single
  // maintenance-page.tpl.php template. So, to have what's done in
  // eclip_preprocess_html() also happen on the maintenance page, it has to be
  // called here.
  eclip_preprocess_html($vars);
}

/**
 * Override or insert variables into the html template.
 */
function eclip_preprocess_html(&$vars) {
  // Toggle fixed or fluid width.
  if (theme_get_setting('eclip_width') == 'fluid') {
    $vars['classes_array'][] = 'fluid-width';
  }
  // Add conditional CSS for IE6.
  drupal_add_css(path_to_theme() . '/fix-ie.css', array('group' => CSS_THEME, 'browsers' => array('IE' => 'lt IE 7', '!IE' => FALSE), 'preprocess' => FALSE));
}

/**
 * Override or insert variables into the html template.
 */
function eclip_process_html(&$vars) {
  // Hook into color.module
  if (module_exists('color')) {
    _color_html_alter($vars);
  }
}

/**
 * Override or insert variables into the page template.
 */
function eclip_preprocess_page(&$vars) {
  // Move secondary tabs into a separate variable.
  $vars['tabs2'] = array(
    '#theme' => 'menu_local_tasks',
    '#secondary' => $vars['tabs']['#secondary'],
  );
  unset($vars['tabs']['#secondary']);

  if (isset($vars['main_menu'])) {
    $vars['primary_nav'] = theme('links__system_main_menu', array(
      'links' => $vars['main_menu'],
      'attributes' => array(
        'class' => array('links', 'inline', 'main-menu'),
      ),
      'heading' => array(
        'text' => t('Main menu'),
        'level' => 'h2',
        'class' => array('element-invisible'),
      )
    ));
  }
  else {
    $vars['primary_nav'] = FALSE;
  }
  if (isset($vars['secondary_menu'])) {
    $vars['secondary_nav'] = theme('links__system_secondary_menu', array(
      'links' => $vars['secondary_menu'],
      'attributes' => array(
        'class' => array('links', 'inline', 'secondary-menu'),
      ),
      'heading' => array(
        'text' => t('Secondary menu'),
        'level' => 'h2',
        'class' => array('element-invisible'),
      )
    ));
  }
  else {
    $vars['secondary_nav'] = FALSE;
  }

  // Prepare header.
  $site_fields = array();
  if (!empty($vars['site_name'])) {
    $site_fields[] = $vars['site_name'];
  }
  if (!empty($vars['site_slogan'])) {
    $site_fields[] = $vars['site_slogan'];
  }
  $vars['site_title'] = implode(' ', $site_fields);
  if (!empty($site_fields)) {
    $site_fields[0] = '<span>' . $site_fields[0] . '</span>';
  }
  $vars['site_html'] = implode(' ', $site_fields);

  // Set a variable for the site name title and logo alt attributes text.
  $slogan_text = $vars['site_slogan'];
  $site_name_text = $vars['site_name'];
  $vars['site_name_and_slogan'] = $site_name_text . ' ' . $slogan_text;
  // Adding custom colors/background
  $vars['customer_header'] = '';
  $vars['footer_text'] = '';
  $vars['is_intranet'] = arg(0) == 'intranet' ? TRUE : FALSE;
  if((arg(0) == 'clip' || arg(0) == 'intranet') && arg(1) != ''){
    $cliente    = load_customer_by_hash(arg(1));
    if(!empty($cliente)){
      if(isset($cliente->field_header['und'][0]['uri']) && $header_uri = $cliente->field_header['und'][0]['uri']){
        $image = theme('image', array('path' => $header_uri));
        $vars['customer_header'] = l($image, arg(0) . '/' . $cliente->field_hash['und'][0]['value'], array('html' => TRUE));        
      }
      if(isset($cliente->field_footer['und'][0]['value']) && $footer = $cliente->field_footer['und'][0]['value']){
        $vars['footer_text'] = $footer;
      }      
      isset($cliente->field_background_color['und'][0]['rgb']) ? drupal_add_css('body { background-color: ' . $cliente->field_background_color['und'][0]['rgb'] . ' !important ;}', array('type' => 'inline')) : '';
      isset($cliente->field_category_background['und'][0]['rgb']) ? drupal_add_css('.category-title { background-color: ' . $cliente->field_category_background['und'][0]['rgb'] . ' !important ; width: 98%; padding: 2px 1%; }', array('type' => 'inline')) : '';
      isset($cliente->field_category_color['und'][0]['rgb']) ? drupal_add_css('.category-title { color: ' . $cliente->field_category_color['und'][0]['rgb'] . ' !important ;}', array('type' => 'inline')) : '';
      isset($cliente->field_link_color['und'][0]['rgb']) ? drupal_add_css('a { color: ' . $cliente->field_link_color['und'][0]['rgb'] . ' !important ;}', array('type' => 'inline')) : '';
      isset($cliente->field_background_color_inner['und'][0]['rgb']) ? drupal_add_css('#contentConteiner { background-color: ' . $cliente->field_background_color_inner['und'][0]['rgb'] . ' !important ;}', array('type' => 'inline')) : '';
      isset($cliente->field_font_color['und'][0]['rgb']) ? drupal_add_css('body {color :' . $cliente->field_font_color['und'][0]['rgb'] . ' !important ;}', array('type' => 'inline')) : '';
      isset($cliente->field_image_border['und'][0]['rgb']) ? drupal_add_css('img { border: 1px solid ' . $cliente->field_image_border['und'][0]['rgb'] . ' !important ;}', array('type' => 'inline')) : '';
      isset($cliente->field_page_border['und'][0]['rgb']) ? drupal_add_css('#mainConteiner { border: 1px solid ' . $cliente->field_page_border['und'][0]['rgb'] . ' !important ;}', array('type' => 'inline')) : '';    
      isset($cliente->field_news_border['und'][0]['rgb']) ? drupal_add_css('.news-item { border: 1px solid ' . $cliente->field_news_border['und'][0]['rgb'] . ' !important ;}', array('type' => 'inline')) : '';
      isset($cliente->field_background_footer['und'][0]['rgb']) ? drupal_add_css('#footer { background-color:  ' . $cliente->field_background_footer['und'][0]['rgb'] . ' !important ;}', array('type' => 'inline')) : '';
      isset($cliente->field_footer_color['und'][0]['rgb']) ? drupal_add_css('#footer { color: ' . $cliente->field_footer_color['und'][0]['rgb'] . ' !important ;}', array('type' => 'inline')) : '';
      
      isset($cliente->field_borde_zona['und'][0]['rgb']) ? drupal_add_css('.destacado, .destacado-2, .destacado-2 .views-field-nothing, .main-left-inner, .main-center-inner, .main-bottom  { border-color: ' . $cliente->field_borde_zona['und'][0]['rgb'] . ' !important ;}', array('type' => 'inline')) : '';
      isset($cliente->field_borde_destacado['und'][0]['rgb']) ? drupal_add_css('.destacado img  { border-color: ' . $cliente->field_borde_zona['und'][0]['rgb'] . ' !important ;}', array('type' => 'inline')) : '';      
      //isset($cliente->field_news_border['und'][0]['rgb']) ? drupal_add_css('.news-item { border-color: ' . $cliente->field_news_border['und'][0]['rgb'] . ' !important ;}', array('type' => 'inline')) : '';
//      
      //drupal_add_css('body {background: url("' . $background . '") !important ;}', array('type' => 'inline'));     
    }
  }
}

/**
 * Helper, get customer by hash
 * @param type $hash
 * @return type 
 */
function load_customer_by_hash($hash){
  $query = db_select('node', 'n');
  $query->innerJoin('field_data_field_hash', 'h', 'h.entity_id = n.nid AND (h.entity_type = :type AND h.deleted = :deleted)', array(':type' => 'node', ':deleted' => 0));
  $query
    ->fields('n', array('nid'))
    ->condition('n.type', 'cliente')
    ->condition('n.status', '1')
    ->condition('h.field_hash_value', $hash);
  $result = $query->execute()->fetchAll();    
  if(isset($result[0]->nid)){
    return node_load($result[0]->nid);
  }
}

/**
 * Override or insert variables into the node template.
 */
function eclip_preprocess_node(&$vars) {
    $vars['submitted'] = $vars['date'] . ' — ' . $vars['name'];
    //dpm($vars['node']);
    if($vars['node']->type == 'clip'){
        $vars['imagen'] = '';
        if(isset($vars['node']->field_imagen_destacado['und'][0])){        
            $field = field_get_items('node', $vars['node'], 'field_imagen_destacado');
            $vars['imagen'] = field_view_value('node', $vars['node'], 'field_imagen_destacado', $field[0], 'default');
        }
        
        $vars['destacados1'] = '';
        if(views_get_view_result('front_destacados', 'block_1', $vars['node']->nid)){
            $vars['destacados1'] = views_embed_view('front_destacados', 'block_1', $vars['node']->nid);
        };        
        
        $vars['destacados2'] = '';
        if(views_get_view_result('front_destacados', 'block_2', $vars['node']->nid)){
            $vars['destacados2'] = views_embed_view('front_destacados', 'block_2', $vars['node']->nid);
        };

        if(isset($vars['node']->field_theme['und'][0]['target_id'])){
            $template = node_load($vars['node']->field_theme['und'][0]['target_id']);
            $vars['template'] = $template->field_machine_name['und'][0]['value'];        
        }else{
            $vars['template'] = 'eclip-01';
        }
    }    
}

/**
 * Override or insert variables into the comment template.
 */
function eclip_preprocess_comment(&$vars) {
  $vars['submitted'] = $vars['created'] . ' — ' . $vars['author'];
}

/**
 * Override or insert variables into the block template.
 */
function eclip_preprocess_block(&$vars) {
  $vars['title_attributes_array']['class'][] = 'title';
  $vars['classes_array'][] = 'clearfix';
}

/**
 * Override or insert variables into the page template.
 */
function eclip_process_page(&$vars) {
  // Hook into color.module
  if (module_exists('color')) {
    _color_page_alter($vars);
  }
}

/**
 * Override or insert variables into the region template.
 */
function eclip_preprocess_region(&$vars) {
  if ($vars['region'] == 'header') {
    $vars['classes_array'][] = 'clearfix';
  }
}
