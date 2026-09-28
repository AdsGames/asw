// ?autorun plays the example's scripted run, as ASW_EXAMPLE_AUTORUN does on
// desktop
Module.preRun = [
  ...[Module.preRun ?? []].flat(),
  () => {
    if (new URLSearchParams(location.search).has("autorun")) {
      Module.ENV.ASW_EXAMPLE_AUTORUN = "1";
    }
  },
];
