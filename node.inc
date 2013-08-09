<?php

/**
 * Common mappings for the Drupal 7 node migrations.
 */
abstract class Os2webNodeMigration extends DrupalNode7Migration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
  }
}


class Os2webContentpageMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {

    parent::__construct($arguments);

    $this->addFieldMapping('field_os2web_base_field_summary', 'field_resume_long');
    $this->addFieldMapping('field_os2web_base_field_summary:summary', 'field_resume_long:summary');
    $this->addFieldMapping('field_os2web_base_field_summary:language', 'language');

    $this->addFieldMapping('field_os2web_base_field_image', 'field_billede');
    $this->addFieldMapping('field_os2web_base_field_image:alt', 'field_billede:alt');
    $this->addFieldMapping('field_os2web_base_field_image:title', 'field_billede:title');
    $this->addFieldMapping('field_os2web_base_field_image:language', 'language');
    $this->addFieldMapping('field_os2web_base_field_image:file_class')
      ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_os2web_base_field_image:preserve_files')
      ->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_lead_img', 'field_lead_image');
    $this->addFieldMapping('field_os2web_base_field_lead_img:alt', 'field_lead_image:alt');
    $this->addFieldMapping('field_os2web_base_field_lead_img:title', 'field_lead_image:title');
    $this->addFieldMapping('field_os2web_base_field_lead:language', 'language');
    $this->addFieldMapping('field_os2web_base_field_lead:file_class')
      ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_os2web_base_field_lead:preserve_files')
      ->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_media', 'field_mediafile');
    $this->addFieldMapping('field_os2web_base_field_media:display', 'field_mediafile:display');
    $this->addFieldMapping('field_os2web_base_field_media:description', 'field_mediafile:description');
    $this->addFieldMapping('field_os2web_base_field_media:language', 'language');
    $this->addFieldMapping('field_os2web_base_field_media:file_class')
      ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_os2web_base_field_media:preserve_files')
      ->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_case', 'field_case');

    $this->addFieldMapping('field_os2web_base_field_doc', 'field_doc');

    $this->addFieldMapping('field_os2web_base_field_contact', 'field_node_reference_kontaktblok')
      ->sourceMigration('os2web_contact_contact');

    $this->addFieldMapping('field_os2web_base_field_edi_tags', 'field_editortags')
      ->sourceMigration('os2web_taxonomies_tax_editor_tag');
    $this->addFieldMapping('field_os2web_base_field_edi_tags:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_edi_tags:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_kle_ref', 'field_termref_kle')
      ->sourceMigration('os2web_taxonomies_tax_kle');
    $this->addFieldMapping('field_os2web_base_field_kle_ref:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_kle_ref:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_loca_ref', 'field_termref_sted')
       ->sourceMigration('os2web_meetings_tax_location');
    $this->addFieldMapping('field_os2web_base_field_loca_ref:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_loca_ref:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_organiz', 'field_organization')
      ->sourceMigration('os2web_taxonomies_tax_org');
    $this->addFieldMapping('field_os2web_base_field_organiz:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_organiz:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_politics', 'field_politics')
      ->sourceMigration('os2web_taxonomies_tax_politics');
    $this->addFieldMapping('field_os2web_base_field_politics:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_politics:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_struct', 'field_site_structure')
      ->sourceMigration('os2web_base_tax_site_structure');
    $this->addFieldMapping('field_os2web_base_field_struct:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_struct:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_hidlinks', 'field_skjul_relaterede_links');

    $this->addFieldMapping('field_os2web_base_field_rotate', 'field_rotate_publish');

    $this->addFieldMapping('field_os2web_base_field_selfserv', 'field_noderef_selvbtjn')
      ->sourceMigration('os2web_base_selfservice_link');

    $this->addFieldMapping('field_os2web_base_field_sourceid', 'field_source_id');

    $this->addFieldMapping('field_os2web_base_field_spotbox', 'field_indholdsside_spotboxe')
      ->sourceMigration('os2web_spotbox_box');

  }

  public function prepareRow($row)
  {

    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
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
    if (!empty($this->arguments['node_migrations'])) {
      // Grabs numbers preceded by 'node/', followed by end of string or '/'
      // eg: 'node/63' -> '63', 'node/17/edit' -> '17', 'node/53ea' -> NULL
      //
      // @todo: extend the lookbehind so it returns NULL for inputs like 'awetnode/34'
      $regex_nid = "/(?<=node\/)[\d]+(?=$|\/)/";

      if (preg_match($regex_nid, $row->field_noderef_selvbtjn[0], $matches)) {
        if ($value = $this->handleSourceMigration($this->arguments['node_migrations'], $matches[0])) {
          $new_path = preg_replace('/' . $matches[0] . '/', $value, $row->field_noderef_selvbtjn[0]);
          if ($new_path) {
            $row->field_noderef_selvbtjn[0] = $new_path;
          }
        }
        return TRUE;
      }
    }
  }
}

class Os2webGalleryMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
  }
}

class Os2webIframeMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
  }
}

class Os2webNewsMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
    $this->addFieldMapping('field_os2web_base_field_rel_subj', 'field_related_tema');

    $this->addFieldMapping('field_os2web_base_field_struct', 'field_site_structure')
      ->sourceMigration('os2web_base_tax_site_structure');
    $this->addFieldMapping('field_os2web_base_field_struct:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_struct:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_summary', 'field_resume_long');
    $this->addFieldMapping('field_os2web_base_field_summary:summary', 'field_resume_long:summary');
    $this->addFieldMapping('field_os2web_base_field_summary:language', 'language');
    $this->addFieldMapping('field_os2web_base_field_sourceid', 'field_source_id');
    $this->addFieldMapping('field_os2web_base_field_add_info', 'field_additional_info');

    $this->addFieldMapping('field_os2web_base_field_lead_img', 'field_lead_image');
    $this->addFieldMapping('field_os2web_base_field_lead_img:alt', 'field_lead_image:alt');
    $this->addFieldMapping('field_os2web_base_field_lead_img:title', 'field_lead_image:title');
    $this->addFieldMapping('field_os2web_base_field_lead_img:file_class')
      ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_os2web_base_field_lead_img:preserve_files')
      ->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_edi_tags', 'field_editortags')
      ->sourceMigration('os2web_taxonomies_tax_editor_tag');
    $this->addFieldMapping('field_os2web_base_field_edi_tags:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_edi_tags:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_kle_ref', 'field_termref_kle')
      ->sourceMigration('os2web_taxonomies_tax_kle');
    $this->addFieldMapping('field_os2web_base_field_kle_ref:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_kle_ref:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_loca_ref', 'field_termref_sted')
      ->sourceMigration('os2web_meetings_tax_location');
    $this->addFieldMapping('field_os2web_base_field_loca_ref:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_loca_ref:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_organiz', 'field_organization')
      ->sourceMigration('os2web_taxonomies_tax_org');
    $this->addFieldMapping('field_os2web_base_field_organiz:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_organiz:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_politics', 'field_politics')
      ->sourceMigration('os2web_taxonomies_tax_politics');
    $this->addFieldMapping('field_os2web_base_field_politics:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_politics:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_promote', 'field_promoteto')
      ->sourceMigration('os2web_base_tax_promoteto');
    $this->addFieldMapping('field_os2web_base_field_promote:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_promote:create_term')->defaultValue(TRUE);


    $this->addFieldMapping('field_os2web_base_field_related', 'field_related');
  }
}

class Os2webRotatingImageMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);

    $this->addFieldMapping('field_os2web_base_field_promote', 'field_promoteto')
      ->sourceMigration('os2web_base_tax_promoteto');
    $this->addFieldMapping('field_os2web_base_field_promote:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_promote:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_rota_img', 'field_rotate_image');
    $this->addFieldMapping('field_os2web_base_field_rota_img:alt', 'field_rotate_image:alt');
    $this->addFieldMapping('field_os2web_base_field_rota_img:title', 'field_rotate_image:title');
    $this->addFieldMapping('field_os2web_base_field_rota_img:language', 'language');
    $this->addFieldMapping('field_os2web_base_field_rota_img:file_class')
      ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_os2web_base_field_rota_img:preserve_files')
      ->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_rota_lnk', 'field_rotating_link');
    $this->addFieldMapping('field_os2web_base_field_rota_lnk:title', 'field_rotating_link:title');
    $this->addFieldMapping('field_os2web_base_field_rota_lnk:attributes', 'field_rotating_link:attributes');

    $this->addFieldMapping('field_os2web_base_field_rotate', 'field_rotate_publish');

    $this->addFieldMapping('field_os2web_base_field_struct', 'field_site_structure')
      ->sourceMigration('os2web_base_tax_site_structure');
    $this->addFieldMapping('field_os2web_base_field_struct:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_struct:create_term')->defaultValue(TRUE);
  }

  public function prepareRow($row)
  {

    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
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
    if (!empty($this->arguments['node_migrations'])) {
      // Grabs numbers preceded by 'node/', followed by end of string or '/'
      // eg: 'node/63' -> '63', 'node/17/edit' -> '17', 'node/53ea' -> NULL
      //
      // @todo: extend the lookbehind so it returns NULL for inputs like 'awetnode/34'
      $regex_nid = "/(?<=node\/)[\d]+(?=$|\/)/";

      if (preg_match($regex_nid, $row->field_rotating_link[0], $matches)) {
        if ($value = $this->handleSourceMigration($this->arguments['node_migrations'], $matches[0])) {
          $new_path = preg_replace('/' . $matches[0] . '/', $value, $row->field_rotating_link[0]);
          if ($new_path) {
            $row->field_rotating_link[0] = $new_path;
          }
        }
        return TRUE;
      }
    }
  }
}

class Os2webSelfserviceLinkMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);

    $this->addFieldMapping('field_os2web_base_field_struct', 'field_site_structure')
      ->sourceMigration('os2web_base_tax_site_structure');
    $this->addFieldMapping('field_os2web_base_field_struct:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_struct:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_ref_node', 'field_attached_node')
      ->sourceMigration('os2web_base_contentpage');
    $this->addFieldMapping('field_os2web_base_field_ref_node:language', 'language');

    $this->addFieldMapping('field_os2web_borger_dk_kle', 'field_termref_kle')
      ->sourceMigration('os2web_taxonomies_tax_kle');
    $this->addFieldMapping('field_os2web_borger_dk_kle:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_borger_dk_kle:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_type', 'field_selvbetjeningslink_type');

    $this->addFieldMapping('field_os2web_base_field_grp_id', 'field_gruppe_id');
    $this->addFieldMapping('field_os2web_base_field_grp_id:language', 'language');

    $this->addFieldMapping('field_os2web_base_field_source', 'field_kilde');
    $this->addFieldMapping('field_os2web_base_field_source:language', 'language');

    $this->addFieldMapping('field_os2web_base_field_direct', 'field_selvbetjening_direkte');
    $this->addFieldMapping('field_os2web_base_field_direct:language', 'language');
  }
}

class Os2webMediaMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
  }
}

class Os2webBorgerDkMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
    $this->addFieldMapping('field_os2web_borger_dk_borgerurl', 'field_os2web_borger_dk_borgerurl');
    $this->addFieldMapping('field_os2web_borger_dk_borgerurl:language', 'language');

    $this->addFieldMapping('field_os2web_borger_dk_byline', 'field_os2web_borger_dk_byline');
    $this->addFieldMapping('field_os2web_borger_dk_byline:language', 'language');

    $this->addFieldMapping('field_os2web_borger_dk_formterm', 'field_os2web_borger_dk_formterm')
      ->sourceMigration('os2web_base_tax_site_structure');
    $this->addFieldMapping('field_os2web_borger_dk_formterm:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_borger_dk_formterm:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_borger_dk_header', 'field_os2web_borger_dk_header');
    $this->addFieldMapping('field_os2web_borger_dk_header:format', 'field_os2web_borger_dk_header:format');
    $this->addFieldMapping('field_os2web_borger_dk_header:language', 'language');

    $this->addFieldMapping('field_os2web_borger_dk_legislati', 'field_os2web_borger_dk_legislati');
    $this->addFieldMapping('field_os2web_borger_dk_legislati:format', 'field_os2web_borger_dk_legislati:format');
    $this->addFieldMapping('field_os2web_borger_dk_legislati:language', 'language');

    $this->addFieldMapping('field_os2web_borger_dk_pagetitle', 'field_os2web_borger_dk_pagetitle');
    $this->addFieldMapping('field_os2web_borger_dk_pagetitle:format', 'field_os2web_borger_dk_pagetitle:format');
    $this->addFieldMapping('field_os2web_borger_dk_pagetitle:language', 'language');

    $this->addFieldMapping('field_os2web_borger_dk_post_text', 'field_os2web_borger_dk_post_text');
    $this->addFieldMapping('field_os2web_borger_dk_post_text:format', 'field_os2web_borger_dk_post_text:format');
    $this->addFieldMapping('field_os2web_borger_dk_post_text:language', 'language');

    $this->addFieldMapping('field_os2web_borger_dk_pre_text', 'field_os2web_borger_dk_pre_text');
    $this->addFieldMapping('field_os2web_borger_dk_pre_text:format', 'field_os2web_borger_dk_pre_text:format');
    $this->addFieldMapping('field_os2web_borger_dk_pre_text:language', 'language');

    $this->addFieldMapping('field_os2web_borger_dk_recommend', 'field_os2web_borger_dk_recommend');
    $this->addFieldMapping('field_os2web_borger_dk_recommend:format', 'field_os2web_borger_dk_recommend:format');
    $this->addFieldMapping('field_os2web_borger_dk_recommend:language', 'language');

    $this->addFieldMapping('field_os2web_borger_dk_selfservi', 'field_os2web_borger_dk_selfservi');
    $this->addFieldMapping('field_os2web_borger_dk_selfservi:format', 'field_os2web_borger_dk_selfservi:format');
    $this->addFieldMapping('field_os2web_borger_dk_selfservi:language', 'language');

    $this->addFieldMapping('field_os2web_borger_dk_shortlist', 'field_os2web_borger_dk_shortlist');
    $this->addFieldMapping('field_os2web_borger_dk_shortlist:format', 'field_os2web_borger_dk_shortlist:format');
    $this->addFieldMapping('field_os2web_borger_dk_shortlist:language', 'language');


    $this->addFieldMapping('field_os2web_borger_dk_kle', 'field_termref_kle')
      ->sourceMigration('os2web_taxonomies_tax_kle');
    $this->addFieldMapping('field_os2web_borger_dk_kle:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_borger_dk_kle:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_borger_dk_image', 'field_billede');
    $this->addFieldMapping('field_os2web_borger_dk_image:alt', 'field_billede:alt');
    $this->addFieldMapping('field_os2web_borger_dk_image:title', 'field_billede:title');
    $this->addFieldMapping('field_os2web_borger_dk_image:language', 'language');
    $this->addFieldMapping('field_os2web_borger_dk_image:file_class')
      ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_os2web_borger_dk_image:preserve_files')
      ->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_spotbox', 'field_indholdsside_spotboxe')
      ->sourceMigration('os2web_spotbox_box');

    $this->addFieldMapping('field_os2web_base_field_contact', 'field_node_reference_kontaktblok')
      ->sourceMigration('os2web_contact_contact');

    $this->addFieldMapping('field_os2web_base_field_selfserv', 'field_noderef_selvbtjn')
      ->sourceMigration('os2web_base_selfservice_link');
   }
}

class OS2webCitizenServiceMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);

    $this->addFieldMapping('field_os2web_citizenservices_img', 'field_lead_image');
    $this->addFieldMapping('field_os2web_citizenservices_img:file_class')
      ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_os2web_citizenservices_img:preserve_files')
      ->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_citizenservices_url', 'field_url');
  }

  public function prepareRow($row)
  {

    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
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
    if (!empty($this->arguments['node_migrations'])) {
      // Grabs numbers preceded by 'node/', followed by end of string or '/'
      // eg: 'node/63' -> '63', 'node/17/edit' -> '17', 'node/53ea' -> NULL
      //
      // @todo: extend the lookbehind so it returns NULL for inputs like 'awetnode/34'
      $regex_nid = "/(?<=node\/)[\d]+(?=$|\/)/";

      if (preg_match($regex_nid, $row->field_url[0], $matches)) {
        if ($value = $this->handleSourceMigration($this->arguments['node_migrations'], $matches[0])) {
          $new_path = preg_replace('/' . $matches[0] . '/', $value, $row->field_url[0]);
          if ($new_path) {
            $row->field_url[0] = $new_path;
          }
        }
        return TRUE;
      }
    }
  }
}

class OS2webContactMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);

    $this->addFieldMapping('field_os2web_contact_field_addr', 'field_stedadresse');
    $this->addFieldMapping('field_os2web_contact_field_addr:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_callc', 'field_kontaktblok_callcenter');
    $this->addFieldMapping('field_os2web_contact_field_show', 'field_kontaktblok_vis_aabnen');

    $this->addFieldMapping('field_os2web_contact_field_dept', 'field_afdeling');
    $this->addFieldMapping('field_os2web_contact_field_dept:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_email', 'field_email');
    $this->addFieldMapping('field_os2web_contact_field_email:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_fricl', 'field_kontaktblok_aa_frelk');
    $this->addFieldMapping('field_os2web_contact_field_fricl:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_friop', 'field_kontaktblok_aa_freaa');
    $this->addFieldMapping('field_os2web_contact_field_friop:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_thucl', 'field_kontaktblok_aa_torlk');
    $this->addFieldMapping('field_os2web_contact_field_thucl:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_thuop', 'field_kontaktblok_aa_toraa');
    $this->addFieldMapping('field_os2web_contact_field_thuop:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_moncl', 'field_kontaktblok_aa_manlk');
    $this->addFieldMapping('field_os2web_contact_field_moncl:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_monop', 'field_kontaktblok_aa_manaa');
    $this->addFieldMapping('field_os2web_contact_field_monop:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_tuecl', 'field_kontaktblok_aa_tirlk');
    $this->addFieldMapping('field_os2web_contact_field_tuecl:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_tueop', 'field_kontaktblok_aa_tiraa');
    $this->addFieldMapping('field_os2web_contact_field_tueop:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_wedcl', 'field_kontaktblok_aa_onsaa');
    $this->addFieldMapping('field_os2web_contact_field_wedcl:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_wedop', 'field_kontaktblok_aa_onslk');
    $this->addFieldMapping('field_os2web_contact_field_wedop:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_mowcl', 'field_kontaktblok_aa_manolk');
    $this->addFieldMapping('field_os2web_contact_field_mowcl:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_mowop', 'field_kontaktblok_aa_manons');
    $this->addFieldMapping('field_os2web_contact_field_mowop:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_satcl', 'field_kontaktblok_aa_loelk');
    $this->addFieldMapping('field_os2web_contact_field_satcl:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_satop', 'field_kontaktblok_aa_loeaa');
    $this->addFieldMapping('field_os2web_contact_field_satop:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_suncl', 'field_kontaktblok_aa_soelk');
    $this->addFieldMapping('field_os2web_contact_field_suncl:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_sunop', 'field_kontaktblok_aa_soeaa');
    $this->addFieldMapping('field_os2web_contact_field_sunop:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_phone', 'field_phone1');

    $this->addFieldMapping('field_os2web_contact_field_zip', 'field_stedpostnr');
    $this->addFieldMapping('field_os2web_contact_field_zip:language', 'language');

    $this->addFieldMapping('field_os2web_contact_field_info', 'field_kontaktblok_egetindhold');
    $this->addFieldMapping('field_os2web_contact_field_info:language', 'language');
    $this->addFieldMapping('field_os2web_contact_field_info:format', 'field_kontaktblok_egetindhold:format');
  }
}

class OS2webCPCaseMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);

    $this->addFieldMapping('field_os2web_cp_service_address', 'field_os2web_cp_service_address');
    $this->addFieldMapping('field_os2web_cp_service_address:language', 'language');

    $this->addFieldMapping('field_os2web_cp_service_case_id', 'field_os2web_cp_service_case_id');
    $this->addFieldMapping('field_os2web_cp_service_case_id:language', 'language');

    $this->addFieldMapping('field_os2web_cp_service_casetype', 'field_os2web_cp_service_casetype');

    $this->addFieldMapping('field_os2web_cp_service_content', 'field_os2web_cp_service_content');
    $this->addFieldMapping('field_os2web_cp_service_content:summary', 'field_os2web_cp_service_content:summary');
    $this->addFieldMapping('field_os2web_cp_service_content:format', 'field_os2web_cp_service_content:format');
    $this->addFieldMapping('field_os2web_cp_service_content:language', 'language');

    $this->addFieldMapping('field_os2web_cp_service_date', 'field_os2web_cp_service_date');

    $this->addFieldMapping('field_os2web_cp_service_doc_ref', 'field_os2web_cp_service_doc_ref')
      ->sourceMigration('os2web_cp_service_cp_document');

    $this->addFieldMapping('field_os2web_cp_service_full_xml', 'field_os2web_cp_service_full_xml');
    $this->addFieldMapping('field_os2web_cp_service_full_xml:language', 'language');

    $this->addFieldMapping('field_os2web_cp_service_key', 'field_os2web_cp_service_key');
    $this->addFieldMapping('field_os2web_cp_service_key:language', 'language');

    $this->addFieldMapping('field_os2web_cp_service_kle_ref', 'field_os2web_cp_service_kle_ref')
      ->sourceMigration('os2web_taxonomies_tax_kle');
    $this->addFieldMapping('field_os2web_cp_service_kle_ref:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_cp_service_kle_ref:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_cp_service_org_unit', 'field_os2web_cp_service_org_unit');

    $this->addFieldMapping('field_os2web_cp_service_property', 'field_os2web_cp_service_property');
    $this->addFieldMapping('field_os2web_cp_service_property:language', 'language');

    $this->addFieldMapping('field_os2web_cp_service_rules', 'field_os2web_cp_service_rules');
    $this->addFieldMapping('field_os2web_cp_service_rules:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_cp_service_rules:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_cp_service_searchmt', 'field_os2web_cp_service_searchmt');
    $this->addFieldMapping('field_os2web_cp_service_searchmt:language', 'language');

    $this->addFieldMapping('field_os2web_cp_service_status', 'field_os2web_cp_service_status');
    $this->addFieldMapping('field_os2web_cp_service_status:language', 'language');

    $this->addFieldMapping('field_os2web_cp_service_sysid', 'field_os2web_cp_service_sysid');
    $this->addFieldMapping('field_os2web_cp_service_sysid:language', 'language');

  }
}

class OS2webCPDocMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);

    $this->addFieldMapping('field_os2web_cp_service_case_id', 'field_os2web_cp_service_case_id');
    $this->addFieldMapping('field_os2web_cp_service_case_id:language', 'language');

    $this->addFieldMapping('field_os2web_cp_service_case_sid', 'field_os2web_cp_service_case_sid');
    $this->addFieldMapping('field_os2web_cp_service_case_sid:language', 'language');

    $this->addFieldMapping('field_os2web_cp_service_content', 'field_os2web_cp_service_content');
    $this->addFieldMapping('field_os2web_cp_service_content:summary', 'field_os2web_cp_service_content:summary');
    $this->addFieldMapping('field_os2web_cp_service_content:format', 'field_os2web_cp_service_content:format');
    $this->addFieldMapping('field_os2web_cp_service_content:language', 'language');

    $this->addFieldMapping('field_os2web_cp_service_date', 'field_os2web_cp_service_date');

    $this->addFieldMapping('field_os2web_cp_service_doc_id', 'field_os2web_cp_service_doc_id');
    $this->addFieldMapping('field_os2web_cp_service_doc_id:language', 'language');

    $this->addFieldMapping('field_os2web_cp_service_doc_type', 'field_os2web_cp_service_doc_type');

    $this->addFieldMapping('field_os2web_cp_service_edited', 'field_os2web_cp_service_edited');

    $this->addFieldMapping('field_os2web_cp_service_file_id', 'field_os2web_cp_service_file_id');
    $this->addFieldMapping('field_os2web_cp_service_file_id:language', 'language');

    $this->addFieldMapping('field_os2web_cp_service_filetype', 'field_os2web_cp_service_filetype');
    $this->addFieldMapping('field_os2web_cp_service_filetype:language', 'language');

    $this->addFieldMapping('field_os2web_cp_service_full_xml', 'field_os2web_cp_service_full_xml');
    $this->addFieldMapping('field_os2web_cp_service_full_xml:language', 'language');

    $this->addFieldMapping('field_os2web_cp_service_key', 'field_os2web_cp_service_key');
    $this->addFieldMapping('field_os2web_cp_service_key:language', 'language');

    $this->addFieldMapping('field_os2web_cp_service_rules', 'field_os2web_cp_service_rules')
      ->sourceMigration('os2web_cp_service_tax_approved_rules');
    $this->addFieldMapping('field_os2web_cp_service_rules:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_cp_service_rules:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_cp_service_sysid', 'field_os2web_cp_service_sysid');
    $this->addFieldMapping('field_os2web_cp_service_sysid:language', 'language');
  }
}

class OS2webBackgroundImageMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
    $this->addFieldMapping('field_os2web_frontend_bg_file', 'field_side_baggrungsbillede');
    $this->addFieldMapping('field_os2web_frontend_bg_file:alt', 'field_side_baggrungsbillede:alt');
    $this->addFieldMapping('field_os2web_frontend_bg_file:title', 'field_side_baggrungsbillede:title');
    $this->addFieldMapping('field_os2web_frontend_bg_file:language', 'language');
    $this->addFieldMapping('field_os2web_frontend_bg_file:file_class')
      ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_os2web_frontend_bg_file:preserve_files')
      ->defaultValue(TRUE);
  }
}

class OS2webHearingsMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
    $this->addFieldMapping('field_os2web_hearings_enddate', 'field_slutdato');

    $this->addFieldMapping('field_os2web_hearings_startdate', 'field_startdato');

    $this->addFieldMapping('field_os2web_hearings_url', 'field_url');
    $this->addFieldMapping('field_os2web_hearings_url:language', 'language');
  }
}

class OS2webKulturnauEventMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);

    $this->addFieldMapping('field_os2web_kulturnaut_descrip', 'field_beskrivelse');
    $this->addFieldMapping('field_os2web_kulturnaut_descrip:summary', 'field_beskrivelse:summary');
    $this->addFieldMapping('field_os2web_kulturnaut_descrip:format', 'field_beskrivelse:format');
    $this->addFieldMapping('field_os2web_kulturnaut_descrip:language', 'language');

    $this->addFieldMapping('field_os2web_kulturnaut_longdesc', 'field_lang_beskrivelse');
    $this->addFieldMapping('field_os2web_kulturnaut_longdesc:language', 'language');

    $this->addFieldMapping('field_os2web_kulturnaut_url', 'field_billeturl');
    $this->addFieldMapping('field_os2web_kulturnaut_url:language', 'language');

    $this->addFieldMapping('field_os2web_kulturnaut_date', 'field_dato');

    $this->addFieldMapping('field_os2web_kulturnaut_genre', 'field_genre');
    $this->addFieldMapping('field_os2web_kulturnaut_genre:language', 'language');

    $this->addFieldMapping('field_os2web_kulturnaut_homepage', 'field_homepage');
    $this->addFieldMapping('field_os2web_kulturnaut_homepage:language', 'language');

    $this->addFieldMapping('field_os2web_kulturnaut_target', 'field_maalgruppe');
    $this->addFieldMapping('field_os2web_kulturnaut_target:language', 'language');

    $this->addFieldMapping('field_os2web_kulturnaut_price', 'field_pris');
    $this->addFieldMapping('field_os2web_kulturnaut_price:language', 'language');

    $this->addFieldMapping('field_os2web_kulturnaut_saddress', 'field_stedadresse');
    $this->addFieldMapping('field_os2web_kulturnaut_saddress:language', 'language');

    $this->addFieldMapping('field_os2web_kulturnaut_sarrang', 'field_stedarrang');
    $this->addFieldMapping('field_os2web_kulturnaut_sarrang:language', 'language');

    $this->addFieldMapping('field_os2web_kulturnaut_shomepag', 'field_stedhomepage');
    $this->addFieldMapping('field_os2web_kulturnaut_shomepag:language', 'language');

    $this->addFieldMapping('field_os2web_kulturnaut_sname', 'field_stednavn');
    $this->addFieldMapping('field_os2web_kulturnaut_sname:language', 'language');

    $this->addFieldMapping('field_os2web_kulturnaut_scity', 'field_stedpostby');
    $this->addFieldMapping('field_os2web_kulturnaut_scity:language', 'language');

    $this->addFieldMapping('field_os2web_kulturnaut_szip', 'field_stedpostnr');
    $this->addFieldMapping('field_os2web_kulturnaut_szip:language', 'language');

    $this->addFieldMapping('field_os2web_kulturnaut_time', 'field_tid');
    $this->addFieldMapping('field_os2web_kulturnaut_time:language', 'language');
  }
}

class OS2webMeetingMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
  }
}

class OS2webMeetingBulletAttMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
  }
}

class OS2webMeetingBulletMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
  }
}

class OS2webOfirJobMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);

    $this->addFieldMapping('field_os2web_ofir_job_logo', 'field_logo');
    $this->addFieldMapping('field_os2web_ofir_job_logo:language', 'language');
    $this->addFieldMapping('field_os2web_ofir_job_logo:file_class')
      ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_os2web_ofir_job_logo:preserve_files')
      ->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_ofir_job_period', 'field_period');

    $this->addFieldMapping('field_os2web_ofir_job_text', 'field_text');
    $this->addFieldMapping('field_os2web_ofir_job_text:format', 'field_text:format');
    $this->addFieldMapping('field_os2web_ofir_job_text:language', 'language');
  }
}

class OS2webSectionsMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);


    $this->addFieldMapping('field_os2web_base_field_struct', 'field_site_structure')
      ->sourceMigration('os2web_base_tax_site_structure');
    $this->addFieldMapping('field_os2web_base_field_struct:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_struct:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_base_field_kle_ref', 'field_termref_kle')
      ->sourceMigration('os2web_taxonomies_tax_kle');
    $this->addFieldMapping('field_os2web_base_field_kle_ref:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_base_field_kle_ref:create_term')->defaultValue(TRUE);
  }
}

class OS2webSpotboxMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);

    $this->addFieldMapping('field_os2web_spotbox_editor_tags', 'field_editortags')
      ->sourceMigration('os2web_taxonomies_tax_editor_tag');
    $this->addFieldMapping('field_os2web_spotbox_editor_tags:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_spotbox_editor_tags:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_spotbox_focus_sect', 'field_fokus_sektionsforside');
    $this->addFieldMapping('field_os2web_spotbox_focus_sect:language', 'language');
    $this->addFieldMapping('field_os2web_spotbox_focus_sect:file_class')
      ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_os2web_spotbox_focus_sect:preserve_files')
      ->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_spotbox_lead_image', 'field_lead_image');
    $this->addFieldMapping('field_os2web_spotbox_lead_image:alt', 'field_lead_image:alt');
    $this->addFieldMapping('field_os2web_spotbox_lead_image:title', 'field_lead_image:title');
    $this->addFieldMapping('field_os2web_spotbox_lead_image:language', 'language');
    $this->addFieldMapping('field_os2web_spotbox_lead_image:file_class')
      ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_os2web_spotbox_lead_image:preserve_files')
      ->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_spotbox_organizati', 'field_organization')
      ->sourceMigration('os2web_taxonomies_tax_org');
    $this->addFieldMapping('field_os2web_spotbox_organizati:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_spotbox_organizati:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_spotbox_politics', 'field_politics')
      ->sourceMigration('os2web_taxonomies_tax_politics');
    $this->addFieldMapping('field_os2web_spotbox_politics:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_spotbox_politics:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_spotbox_promote_to', 'field_promoteto')
      ->sourceMigration('os2web_base_tax_promoteto');
    $this->addFieldMapping('field_os2web_spotbox_promote_to:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_spotbox_promote_to:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_spotbox_sitestruct', 'field_site_structure')
      ->sourceMigration('os2web_base_tax_site_structure');
    $this->addFieldMapping('field_os2web_spotbox_sitestruct:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_spotbox_sitestruct:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_spotbox_image_tall', 'field_spotbox_image_tall');
    $this->addFieldMapping('field_os2web_spotbox_image_tall:alt', 'field_spotbox_image_tall:alt');
    $this->addFieldMapping('field_os2web_spotbox_image_tall:title', 'field_spotbox_image_tall:title');
    $this->addFieldMapping('field_os2web_spotbox_image_tall:language', 'language');
    $this->addFieldMapping('field_os2web_spotbox_image_tall:file_class')
      ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_os2web_spotbox_image_tall:preserve_files')
      ->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_spotbox_video', 'field_spotbox_video');
    $this->addFieldMapping('field_os2web_spotbox_video:alt', 'field_spotbox_video:alt');
    $this->addFieldMapping('field_os2web_spotbox_video:title', 'field_spotbox_video:title');
    $this->addFieldMapping('field_os2web_spotbox_video:language', 'language');
    $this->addFieldMapping('field_os2web_spotbox_video:file_class')
      ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_os2web_spotbox_video:preserve_files')
      ->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_spotbox_big_image', 'field_stort_billede_spot5');
    $this->addFieldMapping('field_os2web_spotbox_big_image:alt', 'field_stort_billede_spot5:alt');
    $this->addFieldMapping('field_os2web_spotbox_big_image:title', 'field_stort_billede_spot5:title');
    $this->addFieldMapping('field_os2web_spotbox_big_image:language', 'language');
    $this->addFieldMapping('field_os2web_spotbox_big_image:file_class')
      ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_os2web_spotbox_big_image:preserve_files')
      ->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_spotbox_ref_kle', 'field_termref_kle')
      ->sourceMigration('os2web_taxonomies_tax_kle');
    $this->addFieldMapping('field_os2web_spotbox_ref_kle:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_spotbox_ref_kle:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_spotbox_ref_place', 'field_termref_sted')
      ->sourceMigration('os2web_meetings_tax_location');
    $this->addFieldMapping('field_os2web_spotbox_ref_place:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_spotbox_ref_place:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_spotbox_text', 'field_text');
    $this->addFieldMapping('field_os2web_spotbox_text:language', 'language');

    $this->addFieldMapping('field_os2web_spotbox_url', 'field_url');
    $this->addFieldMapping('field_os2web_spotbox_url:language', 'language');
  }

  public function prepareRow($row)
  {

    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
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
    if (!empty($this->arguments['node_migrations'])) {
      // Grabs numbers preceded by 'node/', followed by end of string or '/'
      // eg: 'node/63' -> '63', 'node/17/edit' -> '17', 'node/53ea' -> NULL
      //
      // @todo: extend the lookbehind so it returns NULL for inputs like 'awetnode/34'
      $regex_nid = "/(?<=node\/)[\d]+(?=$|\/)/";

      if (preg_match($regex_nid, $row->field_url[0], $matches)) {
        if ($value = $this->handleSourceMigration($this->arguments['node_migrations'], $matches[0])) {
          $new_path = preg_replace('/' . $matches[0] . '/', $value, $row->field_url[0]);
          if ($new_path) {
            $row->field_url[0] = $new_path;
          }
        }
      }
    }
    return TRUE;
  }
}

class OS2webWebformMigration extends Os2webNodeMigration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);
    $this->addFieldMapping('field_os2web_webform_attach', 'field_webform_attachment');
    $this->addFieldMapping('field_os2web_webform_attach:description', 'field_webform_attachment:description');
    $this->addFieldMapping('field_os2web_webform_attach:display', 'field_webform_attachment:display');
    $this->addFieldMapping('field_os2web_webform_attach:language', 'language');
    $this->addFieldMapping('field_os2web_webform_attach:file_class')
      ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_os2web_webform_attach:preserve_files')
      ->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_webform_editortags', 'field_editortags')
      ->sourceMigration('os2web_taxonomies_tax_editor_tag');
    $this->addFieldMapping('field_os2web_webform_editortags:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_webform_editortags:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_webform_kle', 'field_termref_kle')
      ->sourceMigration('os2web_taxonomies_tax_kle');
    $this->addFieldMapping('field_os2web_webform_kle:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_webform_kle:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_webform_org', 'field_organization')
      ->sourceMigration('os2web_taxonomies_tax_org');
    $this->addFieldMapping('field_os2web_webform_org:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_webform_org:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_webform_place', 'field_termref_sted')
      ->sourceMigration('os2web_meetings_tax_location');
    $this->addFieldMapping('field_os2web_webform_place:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_webform_place:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_webform_sitestruct', 'field_site_structure')
      ->sourceMigration('os2web_base_tax_site_structure');
    $this->addFieldMapping('field_os2web_webform_sitestruct:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_webform_sitestruct:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_webform_sofd', 'field_termref_sofd');
    $this->addFieldMapping('field_os2web_webform_sofd:source_type')->defaultValue('tid');
    $this->addFieldMapping('field_os2web_webform_sofd:create_term')->defaultValue(TRUE);

    $this->addFieldMapping('field_os2web_webform_summary', 'field_resume_long');
    $this->addFieldMapping('field_os2web_webform_summary:summary', 'field_resume_long:summary');
    $this->addFieldMapping('field_os2web_webform_summary:format', 'field_resume_long:format');
    $this->addFieldMapping('field_os2web_webform_summary:language', 'language');
  }
}
