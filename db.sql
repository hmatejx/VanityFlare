CREATE TABLE `SVP` (
  `id` int(9) unsigned NOT NULL AUTO_INCREMENT,
  `len` tinyint(1) unsigned NOT NULL,
  `suffix` varchar(16) NOT NULL,
  `xiffus` varchar(16) NOT NULL,
  `address` char(56) NOT NULL,
  `seed` char(56) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `idxaddr` (`address`),
  UNIQUE KEY `idxseed` (`seed`),
  KEY `idxlen` (`len`),
  KEY `idxxif` (`xiffus`),
  KEY `idxsuf` (`suffix`)
) ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=latin1;


CREATE TABLE `keywords` (
  `id` int(7) unsigned NOT NULL AUTO_INCREMENT,
  `keyword` varchar(16) NOT NULL,
  `type` varchar(5) NOT NULL,
  `pool` tinyint(1) unsigned NOT NULL,
  `len` tinyint(1) unsigned NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `idxkey` (`keyword`),
  KEY `idxpool` (`pool`),
  KEY `idxtype` (`type`),
  KEY `idxlen` (`len`)
) ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=latin1;
