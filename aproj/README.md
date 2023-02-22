= aproj notes

== aproj.cpp

This function calls all `aproj-*` executables like this: `aproj-<subcommand> --brief` to capture the brief description from the subcommand for it's help.

When `aproj <subcommand> <options>` is called, `aproj` calls the subcommand like this:
`aproj-<subcommand> <options> --indirect="<subcommand>`. This ensures the subcommand can populate it's help correctly.

== aproj-<subcomand>.cpp

These files should all call `common_init()` to ensure `CLI::App` receives the subcomand's hidden brief and indirect options.
