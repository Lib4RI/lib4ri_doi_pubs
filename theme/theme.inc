<?php
/**
 * @file
 * Theme hooks.
 */



/**
 * Implements hook_preprocess_lib4ridora_citation_solr_results().
 */
function template_preprocess_lib4ri_doi_pubs_citation_solr_results(&$variables) {
    module_load_include('inc', 'lib4ri_doi_pubs', '/includes/functions');
    module_load_include('inc', 'lib4ridora', 'theme/theme');
    template_preprocess_lib4ridora_citation_solr_results($variables);
    foreach ($variables['citations'] as &$citation) {
        if (isDoiUnpublished($citation['pdf_listing']['#object'])){
            $citation['unreleased'] = 'Unrelease DOI publication';
        }
        else{
            $citation['unreleased'] = null;
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

