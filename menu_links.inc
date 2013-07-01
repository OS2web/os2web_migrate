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
    }
  }
}
