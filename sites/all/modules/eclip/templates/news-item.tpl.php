<div class="noticia clearfix <?php $imagen ? print 'con-imagen' : print 'sin-imagen'; ?>">
	<?php if($imagen) : ?>
	<div class="image"><img src="<?php print $imagen ?>"/></div>
	<?php endif; ?>	
	<div class="title" style="font-size: <?php print $colores['title_size'] ?>; font-weight: <?php print $colores['title_weight'] ?>"><?php print l($titulo, $link, array('absolute' => TRUE, 'attributes' => array('style' => 'color:' . $colores['link_color'] . '; text-decoration:' . $colores['title_decoration'] . '; font-family
	:' . $colores['title_font']))) ?></div>	
	<div class="medio-fecha" style="font-family: <?php print $colores['title_font'] ?>"><strong><?php print $medio; ?> - <?php print $fecha; ?></strong></div>
	<div class="description" style="font-family: <?php print $colores['title_font'] ?>"><?php print $bajada ?></div>
</div>