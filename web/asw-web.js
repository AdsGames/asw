// ASW web runtime. asw_add_web_target() adds it to web builds with
// --extern-pre-js, so it runs before the Emscripten code and sees the first
// loading status. Hooks that the page already set on Module still run.
//
// In an iframe, the game posts its loading state to the parent page:
//   { type: "asw:status", text }  Emscripten status, "" when loading is done
//   { type: "asw:ready" }         The runtime started
// The parent page then shows the status, so the page's own #status stays
// hidden.
var Module = typeof Module != "undefined" ? Module : {};

(() => {
  const framed = window.parent !== window;
  const canvas = Module.canvas ?? document.getElementById("canvas");
  const status = document.getElementById("status");

  Module.canvas = canvas;
  if (status && framed) {
    status.hidden = true;
  }

  const notifyParent = (message) => {
    if (framed) {
      window.parent.postMessage(message, "*");
    }
  };

  // Run the ASW hook, then the one the page set, if any
  const hook = (name, fn) => {
    const pageHook = Module[name];
    Module[name] = (...args) => {
      fn(...args);
      pageHook?.(...args);
    };
  };

  // Keys go to the game, not to page scrolling
  window.addEventListener("keydown", (event) => {
    const keys = ["ArrowUp", "ArrowDown", "ArrowLeft", "ArrowRight", "Tab"];
    if (keys.includes(event.key)) {
      event.preventDefault();
    }
  });
  // Space scrolls on keypress. Cancelling it on keydown would stop the
  // keypress SDL reads typed text from, so text boxes could not get a space.
  window.addEventListener("keypress", (event) => {
    if (event.key === " ") {
      event.preventDefault();
    }
  });
  canvas?.addEventListener("pointerdown", () => canvas.focus());

  hook("setStatus", (text) => {
    if (status) {
      status.textContent = text;
      status.hidden = framed || !text;
    }
    notifyParent({ type: "asw:status", text });
  });

  hook("onRuntimeInitialized", () => {
    if (status) {
      status.hidden = true;
    }
    canvas?.focus();
    notifyParent({ type: "asw:ready" });
  });

  // The game called asw::core::exit()
  hook("onStop", () => {
    if (status) {
      status.textContent = "Stopped. Click to restart.";
      status.hidden = false;
    }
    document.body.style.cursor = "pointer";
    document.body.addEventListener("click", () => location.reload(), {
      once: true,
    });
  });
})();
