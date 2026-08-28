# mod-craftingspeed for Tortoise

English | Español

## Description

This module allows server administrators to globally modify the cast time of crafting spells. It identifies spells that consume reagents and create or enchant items, and applies a configurable multiplier to their casting time.

This is an adaptation of the AzerothCore mod-crafting-casttime for the Tortoise core.

## Credits & Origin

- **Port Source**: [FiraVT/Craftingspeed](https://github.com/FiraVT/Craftingspeed)
- **Original Source**: [Day36512/mod-craftspeed](https://github.com/Day36512/mod-craftspeed)
- **Original Author & Concept**: Day36512
- **Original AzerothCore Module**: mod-crafting-casttime
- **Port Maintainer**: FiraVT
- **Ported to Tortoise**: Penqle & AI
- **AzerothCore Community**: Original logic and concepts

## Features

- **Global Cast Time Multiplier**: Easily speed up or slow down crafting across the entire server.
- **Smart Spell Discovery**: Automatically identifies crafting, enchanting, and trade skill spells.
- **DBC-Integrated Scaling**: Matches target cast times to the closest available values in the server's `SpellCastTimes` data for maximum compatibility.
- **Multi-language Support**: Includes English and Spanish translations for player notifications (extensible via `module_string` tables).
- **Configurable Notification**: Optional system message on login to inform players of the current crafting speed.
- **Safe Implementation**: Uses a startup hook to modify spell data in memory once, ensuring no permanent changes to your database or DBC files.

## Important Differences from AzerothCore Version

This port includes several adaptations specifically for the Tortoise / Tortoise-core environment:

- **Core API**: Adapted to use Tortoise-core's `sConfig` and `SpellEntry` structure.
- **Cast Time Logic**: AzerothCore allows cloning `CastTimeEntry` pointers; Tortoise uses a fixed-size `sSpellCastTimesStore`. This module smartly iterates the store to find the best match for the desired multiplier.
- **Localization**: Uses the `module_string` and `module_string_locale` database tables.
- **Vanilla Compatibility**: Logic tailored for the 1.12.1 object model.

## Requirements

- Tortoise core.
- MariaDB or MySQL database.
- Compiler with C++17 support.

## Installation

### 1. Clone the Module

Navigate to your Tortoise `modules/` directory:

```bash
cd <TortoiseDir>/modules
git clone https://github.com/FiraVT/Craftingspeed.git mod-craftingspeed
```

### 2. Compile

Re-configure and build your project:

```bash
cd <TortoiseDir>/build
cmake .. -DMODULES=static
cmake --build . --target mangosd --config Release
```

### 3. Database Updates

The module includes SQL migrations that are automatically processed by the Tortoise DB auto-updater if `Database.AutoUpdate.AllowedModules` is set to "all" (or includes `mod-craftingspeed`) in your `mangosd.conf`.

If you have auto-updates disabled, you can manually import the SQL located in `data/sql/world/` into your `tw_world` database:

```sql
SOURCE modules/mod-craftingspeed/data/sql/world/mod_craftingspeed_module_string.sql;
```

### 4. Configure

Copy `modules/mod-craftingspeed/conf/mod-craftingspeed.conf.dist` to your server's `modules/` directory and rename it to `mod-craftingspeed.conf`.

```ini
###################################################################################################
#    CRAFTING SPEED MODULE CONFIGURATION
###################################################################################################

# Enable or disable the module globally (1 = Enabled, 0 = Disabled)
CraftingCastTime.Enable = 1

# Multiplier for the cast time of crafting spells.
# (e.g., 0.25 = 25% of normal cast time, 0.0 = Instant)
CraftingCastTime.Multiplier = 0.25

# Show informational message on player login (1 = Show, 0 = No message)
CraftingCastTime.ShowMessageOnLogin = 1
```

## License

This module is released under the GNU AGPL v3 License.

