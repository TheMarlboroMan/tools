# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog] and this project attempts to adhere to [Semantic Versioning].

Changes will be documented under Added, Changed, Deprecated, Removed, Fixed or Security headers.

## Known bugs:

## Unreleased
### Pending

## [v1.1.5]: 2024-07-28
### Changed
- Pager can now handle page changes when setting index.
- Previous change applied by default to lists.

## [v1.1.4]: 2024-07-24
### Added
- Added small time library.

## [v1.1.3]: 2024-03-08
### Added
- Added many json methods to the json tools and json config file to be able to manipulate them better.

## [v1.1.2]: 2024-02-11
### Changed
- debug methods will exist in production lib, but will throw.

## [v1.1.1]: 2024-02-08
### Added
- changes build and versioning.

## [v1.1.0]: 2022-08-26
### Added
- json_config_file::has_path
- json_config_file::add

## [v1.0.0]: 2022-08-24
### Added
- First version of this file.
- tools::get_lib_version.
- tools::filesystem namespace added for fs compatibility with gcc7.5.

### Changed
- dump_file uses a stringstream now.

### Removed
- tools::file_exists
