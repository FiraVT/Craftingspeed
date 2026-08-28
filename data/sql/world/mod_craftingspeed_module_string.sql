DELETE FROM `module_string` WHERE `module`='mod-craftingspeed' AND `id`=1;
INSERT INTO `module_string` (`module`, `id`, `content_default`) VALUES
('mod-craftingspeed', 1, 'Crafting speed module enabled. Multiplier: %.2f');

DELETE FROM `module_string_locale` WHERE `module`='mod-craftingspeed' AND `id`=1;
INSERT INTO `module_string_locale` (`module`, `id`, `locale`, `content`) VALUES
('mod-craftingspeed', 1, 6, 'Modulo de velocidad de artesania habilitado. Multiplicador: %.2f');
