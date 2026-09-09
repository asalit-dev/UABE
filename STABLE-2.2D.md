# UABE 2.2 stable d

## Decision

The repository contains the later open-source rework (`v3.0-beta1` and newer), not the historical source that produced the original UABE 2.2 stable d interface. The original stable d interface is therefore not reproducibly rebuildable from this repository alone.

The reliable way to obtain the exact 2.2 stable d appearance and behavior is to use the official 32-bit release published by [SeriousCache/UABE](https://github.com/SeriousCache/UABE/releases/tag/2.2stabled).

## Automated package

Run the GitHub Actions workflow **Official UABE 2.2 stable d (Windows x86)** manually. It downloads the official `AssetsBundleExtractor_2.2stabled_32bit.zip`, verifies its SHA-256, and publishes the result as a workflow artifact.

The workflow does not pretend to rebuild unavailable historical source. This avoids shipping a beta/rework executable under the stable d name and preserves the exact original user interface.

## SHA-256

The official 32-bit release archive used by the workflow has SHA-256:

```text
0ac09af0774dfebe183aa9de43fe682a80a397477a490c82b7304f5c7153f1ff
```

The official executable inside the extracted package has SHA-256:

```text
4e353e1d3454e16b39338a1839e71fd732dcef3f9ca7d7821088e576917a131e
```

## Branches

- `master`: open-source rework and Windows x86 build work.
- `recreate-2.2stabled`: experimental rework using the stable d class database and selected UI adjustments. It is **not** the original stable d source.
- `legacy`: historical documentation only; it does not contain the old implementation source.
