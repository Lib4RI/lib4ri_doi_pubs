<?php

function isDoiUnpublished($object){
    if (!isset($object)){
        return FALSE;
    }
    
    $staging_collection = variable_get('lib4ri_doi_pubs_doi_collection');
    $parents = $object->getParents();
    if (sizeof($parents) == 1){
        if ($parents[0] == $staging_collection){
            return TRUE;
        }
    }
    return FALSE;
}
