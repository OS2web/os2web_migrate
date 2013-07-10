<?php


/**
 * Common mappings for the Drupal 7 taxonomy migrations.
 */
abstract class TermMigration extends DrupalTerm7Migration {
  public function __construct(array $arguments) {
    $this->sourceFields['language'] = 'und';
    parent::__construct($arguments);

    $this->removeFieldMapping('language');
    $this->addFieldMapping('language')
      ->defaultValue('und');

  }
}


class Os2webSiteStructMigration extends TermMigration {
  public function __construct(array $arguments) {

    parent::__construct($arguments);


    $this->addFieldMapping('field_os2web_base_field_ref_node', 'field_attached_node')
      ->sourceMigration('os2web_base_contentpage');

    $this->addFieldMapping('field_os2web_base_field_ref_focu', 'field_attached_node_fokus')
      ->sourceMigration('os2web_spotbox_box');

    $this->addFieldMapping('field_os2web_base_field_issubsit', 'field_is_subsite');

    $this->addFieldMapping('field_os2web_base_field_banner', 'field_subsite_banner');
    $this->addFieldMapping('field_os2web_base_field_banner:alt', 'field_subsite_banner:alt');
    $this->addFieldMapping('field_os2web_base_field_banner:title', 'field_subsite_banner:title');
    $this->addFieldMapping('field_os2web_base_field_banner:language', 'language');
    $this->addFieldMapping('field_os2web_base_field_banner:file_class')
      ->defaultValue('MigrateFileFid');

    $this->addFieldMapping('field_os2web_base_field_logo', 'field_subsite_logo');
    $this->addFieldMapping('field_os2web_base_field_logo:alt', 'field_subsite_logo:alt');
    $this->addFieldMapping('field_os2web_base_field_logo:title', 'field_subsite_logo:title');
    $this->addFieldMapping('field_os2web_base_field_logo:language', 'language');
    $this->addFieldMapping('field_os2web_base_field_logo:file_class')
      ->defaultValue('MigrateFileFid');

    $this->addFieldMapping('field_os2web_base_field_contact', 'field_node_reference_kontaktblok')
      ->sourceMigration('os2web_contact_contact');

  }
}
