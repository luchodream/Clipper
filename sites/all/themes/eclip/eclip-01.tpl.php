<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
?>
<?php if($op == 'view') : // el clip se esta viendo (no esta siendo editado) ?>
<?php $news = _eclip_get_news_by_clip($node->nid); ?>
<div class="destacado eclip-01">
    <table style="width: 100%">
      <tbody>
        <tr>
          <td style="width: 60%; vertical-align: top; ">
            <?php print $destacados1; ?>
          </td>
          <td style="width: 40%; vertical-align: middle; text-align: center;">
            <?php if ($imagen) print render($imagen); ?>
          </td>              
        </tr>
      </tbody>
    </table>
</div>
<?php if($destacados2): ?>
<div class="destacado-2 eclip-01">
    <?php print $destacados2; ?>
    <div class="clear-both"></div>
</div>
<?php endif; ?>
<div class="main-template eclip-01 view">
  <div class="main-template-inner">
    <div class="main-left">
      <div class="main-left-inner">
        <div class="zone-1"><?php isset($node->nid) ? print _eclip_build_news_by_zone($news, 1) : ''; ?></div>
      </div>      
    </div>
    <div class="main-right">
      <div class="main-right-inner">
        <div class="zone-2"><?php isset($node->nid) ? print _eclip_build_news_by_zone($news, 2) : ''; ?></div>
        <div class="clipper-archive"><?php print _eclip_build_archive_clips(arg(1)); ?></div>
      </div>        
    </div>
    <div class="clear-both"></div>
  </div>
</div>
<?php endif; ?>
