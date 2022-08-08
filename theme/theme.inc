<?php
/**
 * @file
 * Theme hooks.
 */



/**
 * Implements hook_preprocess_lib4ridora_citation_solr_results().
 */
function template_preprocess_lib4ri_doi_pubs_citation_solr_results(&$variables) {
    module_load_include('inc', 'lib4ri_staged_ingest', 'includes/utils');
    module_load_include('inc', 'lib4ridora', 'theme/theme');
    template_preprocess_lib4ridora_citation_solr_results($variables);
    foreach ($variables['citations'] as &$citation) {
        if (isDoiStaged($citation['pdf_listing']['#object'])){
            $citation['staged'] = 'Staged DOI publication';
        }
        else{
            $citation['staged'] = null;
        }
    }
}

/**
 * Implements hook_process_lib4ridora_citation_solr_results().
 */
function template_process_lib4ri_doi_pubs_citation_solr_results(&$variables) {
    module_load_include('inc', 'lib4ridora', 'theme/theme');
    template_process_lib4ridora_citation_solr_results($variables);
}

function isDoiStaged($object){
    if (!isset($object)){
        return FALSE;
    }
    
    $staging_collection = variable_get('lib4ri_doi_pubs_collection');
    $parents = $object->getParents();
    if (sizeof($parents) == 1){
        if ($parents[0] == $staging_collection){
            return TRUE;
        }
    }
    return FALSE;
}