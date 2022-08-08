<?php

function lib4ri_doi_pubs_admin_form() {
    $form['lib4ri_doi_pubs_doi_collection'] = array(
        '#type' => 'textfield',
        '#autocomplete_path' => 'islandora/basic_collection/find_collections',
        '#title' => t('DOI collection'),
        '#default_value' => variable_get('lib4ri_doi_pubs_doi_collection', ''),
        '#required' => FALSE,
    );
    
    $form['lib4ri_doi_pubs_live_collection'] = array(
        '#type' => 'textfield',
        '#autocomplete_path' => 'islandora/basic_collection/find_collections',
        '#title' => t('Live collection'),
        '#default_value' => variable_get('lib4ri_doi_pubs_live_collection', ''),
        '#required' => FALSE,
    );
        
    return system_settings_form($form);
}


function lib4ri_doi_pubs_migrate_form($form, &$form_state, $islandora_object) {
    $form['description'] = array(
        '#type' => 'item',
        '#title' => t('Migrate this item'),
    );
    $form['live_collection'] = array(
        '#autocomplete_path' => 'islandora/basic_collection/find_collections',
        '#default_value' => explode(',',variable_get('lib4ri_doi_pubs_live_collection'))[0],
        '#type' => 'textfield',
        '#title' => t('Target Collection'),
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Unstage Object',
    );
    $form_state['doi_object'] = $islandora_object->id;
    return $form;
}


function lib4ri_doi_pubs_migrate_form_submit($form, &$form_state) {
    module_load_include('inc', 'islandora_basic_collection', 'includes/utilities');
    
    $object = islandora_object_load($form_state['doi_object']);
    $new_collection = islandora_object_load($form_state['values']['live_collection']);
    $current_parents = islandora_basic_collection_get_parent_pids($object);
    if ($object && $new_collection) {
        global $user;
        $object->owner = $user->name;
        islandora_basic_collection_add_to_collection($object, $new_collection);
        $message = t('The object @object has been moved to collection @collection', array(
            '@object' => $object->label,
            '@collection' => $new_collection->label));
        
        lib4ri_doi_pubs_set_policy($object, $new_collection);
        
        foreach ($current_parents as $parents) {
            $parent = islandora_object_load($parents);
            islandora_basic_collection_remove_from_collection($object, $parent);
        }

        drupal_set_message($message);
    }
}

function lib4ri_doi_pubs_migrate_form_validate($form, &$form_state) {
    $new_collection = islandora_object_load($form_state['values']['live_collection']);
    $collection_models = islandora_basic_collection_get_collection_content_models();
    $is_a_collection = FALSE;
    if (is_object($new_collection)) {
        $is_a_collection = (
            (count(array_intersect($collection_models, $new_collection->models)) > 0) &&
            isset($new_collection['COLLECTION_POLICY'])
            );
    }
    if (!$is_a_collection) {
        form_set_error('live_name', t('Not a valid collection'));
    }
    $has_ingest_permissions = islandora_object_access(ISLANDORA_INGEST, $new_collection);
    if (!$has_ingest_permissions) {
        form_set_error('live_name', t('You do not have permission to ingest objects to this collection'));
    }
}

function lib4ri_doi_pubs_set_policy($object, $collection){
    
    /* Getting collection's policy */
    $c_xacml = new Xacml($collection['POLICY']->content);
    $o_xacml = new IslandoraXacml($object);
    
    if ($c_xacml->viewingRule->isPopulated()) {
        $o_xacml->viewingRule->clear();
        $o_xacml->viewingRule->addUser($c_xacml->viewingRule->getUsers());
        $o_xacml->viewingRule->addRole($c_xacml->viewingRule->getRoles());
    }
    
    if ($c_xacml->managementRule->isPopulated()) {
        $o_xacml->managementRule->clear();
        $o_xacml->managementRule->addUser($c_xacml->managementRule->getUsers());
        $o_xacml->managementRule->addRole($c_xacml->managementRule->getRoles());
    }
    $o_xacml->writeBackToFedora();
}