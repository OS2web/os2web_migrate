<?php
/**
 * @file
 * Implementation of DrupalMenuLinksMigration.
 */

/*
 * Class for menu link migrations: from {menu_links} into {menu_links}.
 */
class Os2webMenuLinksMigration extends DrupalMenuLinksMigration {
  /**
   * @param array $arguments
   *     $arguments['menu_migration'] should be defined -- menu links are normally
   *        dependent on menus.
   */
  public function __construct(array $arguments) {
    parent::__construct($arguments);

    // Most mappings are straightforward
    $this->addSimpleMappings(array('router_path', 'link_title',
        'module', 'hidden', 'external', 'has_children',
        'expanded', 'weight', 'depth', 'customized', 'p1', 'p2', 'p3', 'p4',
        'p5', 'p6', 'p7', 'p8', 'p9', 'updated'));

    $this->addFieldMapping('menu_name', 'menu_name');

    // plid is taken from this migration. If the parent hasn't yet been migrated,
    // we need to create a stub.
    $this->addFieldMapping('plid', 'plid')
         ->sourceMigration($this->getMachineName());

    $this->addFieldMapping('link_path', 'link_path');

    // Ran into trouble with options, so I'm leaving it out for now
    $this->addUnmigratedSources(array('options'));
    $this->addUnmigratedDestinations(array('options'));
  }

  /**
   * Query for the basic menu_links data.
   *
   * @return QueryConditionInterface
   */
  protected function query() {
    $query = Database::getConnection('default', $this->sourceConnection)
             ->select('menu_links', 'm')
             ->fields('m');
    return $query;
  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param $row
   */
  public function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    // Map old menu-name to new one.
    switch ($row->menu_name) {
      case 'menu-erhverv-genveje':
      case 'menu-erhverv-genveje2':
        $row->menu_name = 'os2web-menu-business-shortcuts';
        break;
      case 'menu-indholdsmenu':
        $row->menu_name = 'os2web-menu-contentmenu';
        break;
      case 'menu-andre-netsteder':
        $row->menu_name = 'os2web-menu-other-sites';
        break;
      case 'menu-erhverv-genveje2':
        $row->menu_name = 'os2web-menu-business-shortcuts';
        break;

      // All menus we dont want to be migrated from source.
      case 'management':
      case 'menu-top-menu':
      case 'navigation':
      case 'user-menu':
        return FALSE;
        break;
    }


    /*
     * Fun with link_path!
     *
     * check for nids
     *   if pattern 'node/nid', replace nid with handleSourceMigration($node_migrations, $nid)
     * check for tids
     *   if pattern 'term/tid', replace tid with handleSourceMigration($term_migrations, $tid)
     * check for potential alias (after basepath, I believe)
     *   if (handleSourceMigration($path_migration, $potentialalias), replace potentialalias with result
     */

    $matches = array();
    $value = '';

    // NIDs first
    if (!empty($this->node_migrations)) {
      // Grabs numbers preceded by 'node/', followed by end of string or '/'
      // eg: 'node/63' -> '63', 'node/17/edit' -> '17', 'node/53ea' -> NULL
      //
      // @todo: extend the lookbehind so it returns NULL for inputs like 'awetnode/34'
      $regex_nid = "/(?<=node\/)[\d]+(?=$|\/)/";

      if (preg_match($regex_nid, $row->link_path, $matches)) {
        if ($value = $this->handleSourceMigration($this->node_migrations, $matches[0])) {
          $new_path = preg_replace('/' . $matches[0] . '/', $value, $row->link_path);
          if ($new_path) {
            $row->link_path = $new_path;
          }
        }
        return TRUE;
      }
    }

    $matches = array();
    $value = '';

    // TIDs next
    if (!empty($this->term_migrations)) {
      // Grabs numbers preceded by 'node/', followed by end of string or '/'
      // eg: 'taxonomy/term/63' -> '63', 'taxonomy/term/17/edit' -> '17', 'taxonomy/term/53ea' -> NULL
      //
      // @todo: extend the lookbehind so it returns NULL for inputs like 'awetnode/34'
      $regex_tid = "/(?<=taxonomy\/term\/)[\d]+(?=$|\/)/";

      if (preg_match($regex_tid, $row->link_path, $matches)) {
        if ($value = $this->handleSourceMigration($this->term_migrations, $matches[0])) {
          $new_path = preg_replace('/' . $matches[0] . '/', $value, $row->link_path);
          if ($new_path) {
            $row->link_path = $new_path;
          }
        }
        return TRUE;
      }
    }

    return TRUE;
  }
}
