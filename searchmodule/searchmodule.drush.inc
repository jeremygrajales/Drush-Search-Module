<?php
// $Id:

/**
 * @file Drush Search Module commands
 */

/**
 * Implementation of hook_drush_command().
 */
function searchmodule_drush_command()
{
  $items['search-module'] = array
  (
    'callback' => 'general',
    'description' => 'Search modules',
    'example' => array
    (
       'drush module-search views' => 'The search query is "views". The output will show the first page of results from drupal.org.',
    ),
    'arguments' => array(
      'module' => 'Module name.',
     ),
    'options' => array
     (
       'page' => 'pages in result',
     ),
     'aliases' => array('sm'),
     'bootstrap' => DRUSH_BOOTSTRAP_MAX,
    );
   return $items;
}

/**
 * Search modules with shell and print result
 */
function general($module, $page=0) {

  // Get results
  list($results, $numProjects, $projectNames, $projectTitles) = array_values(getResults($module, $page));

  // Show results
  echo "\n";
  echo "Searching for modules on drupal.org...\n";
  echo "Found $numProjects modules containing the words: $module\n\n";
  print_r($results);
  echo "\n";

  // Get user input
  do {
    $n = drush_prompt(dt('Enter a number to download project (drush dl)'));
  } while (!isset($projectNames[intval($n-1)]));

  // Download selection with drush dl
  echo "Downloading module \"" . $projectTitles[intval($n-1)] . "\"\n\n";
  exec("drush dl " . $projectNames[intval($n-1)]);
}

function getResults($module, $page) {

  // Retrieve first results page
  $c = curl_init();
  curl_setopt($c, CURLOPT_URL, "https://www.drupal.org/search/site/$module?f%5B0%5D=ss_meta_type%3Amodule&f%5B4%5D%3Dsm_field_project_type%3Afull&page=$page");
  curl_setopt($c, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($c, CURLOPT_SSL_VERIFYPEER, false);
  $html = curl_exec($c);
  if(curl_error($c))
    die("Couldn\'t retreive data.");

  $resultProjects = array();
  $numProjects = 0;
  // Find projects by hyperlinks
  preg_match_all('/<a href="https:\/\/www.drupal.org\/project\/(.*?)">(.*?)<\/a>/', $html, $projects);
  // Get number of available projects from results page
  preg_match('/Found ([0-9]+) results containing the words:/', $html, $numProjects);
  $numProjects = $numProjects[1];
  for($i=0; $i<count($projects[1]); $i++) {
    $resultProjects[] = str_pad("[".($i+1)."] ", 7) . str_pad($projects[2][$i], 35) . " \t" . str_pad($projects[1][$i], 35) . "\n";
  }
  // Return results
  return array('results' => implode($resultProjects), 'numProjects' => $numProjects, 'projectNames' => $projects[1], 'projectTitles' => $projects[2]);
}


?>
