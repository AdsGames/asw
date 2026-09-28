// Take a screenshot of each browser example at the end of its scripted run.
//
// Usage: node screenshots.cjs <url> <dir>
//   url  Where the packaged examples are served, for example
//        http://localhost:8000 (each example is at <url>/<name>/)
//   dir  The packaged examples folder. screenshot.png is written into each
//        example's folder.
//
// Needs playwright-core. Uses the installed Chrome, or CHROME_PATH if set.

const { chromium } = require("playwright-core");
const fs = require("fs");
const path = require("path");

const TIMEOUT_MS = 60000;

async function main() {
  const [url, dir] = process.argv.slice(2);
  if (!url || !dir) {
    console.error("Usage: node screenshots.cjs <url> <dir>");
    process.exit(1);
  }

  const names = fs
    .readdirSync(dir, { withFileTypes: true })
    .filter((entry) => entry.isDirectory())
    .map((entry) => entry.name)
    .sort();

  const browser = await chromium.launch({
    ...(process.env.CHROME_PATH
      ? { executablePath: process.env.CHROME_PATH }
      : { channel: "chrome" }),
    args: [
      "--use-angle=swiftshader",
      "--enable-unsafe-swiftshader",
      "--autoplay-policy=no-user-gesture-required",
    ],
  });

  let failed = false;

  for (const name of names) {
    const page = await browser.newPage({
      viewport: { width: 800, height: 600 },
    });
    const errors = [];
    page.on("pageerror", (error) => errors.push(String(error)));

    await page.goto(`${url}/${name}/?autorun`);

    // The scripted run ends with asw::core::exit(), which stops the loop and
    // shows the restart message. The last frame stays on the canvas.
    try {
      await page.waitForFunction(
        () => {
          const status = document.getElementById("status");
          return !status.hidden && status.textContent.startsWith("Stopped");
        },
        null,
        { timeout: TIMEOUT_MS },
      );
    } catch {
      errors.push(`scripted run did not finish in ${TIMEOUT_MS / 1000}s`);
    }

    if (errors.length > 0) {
      failed = true;
      console.error(`${name}: ${errors.join("; ")}`);
    } else {
      await page.evaluate(() => {
        document.getElementById("status").hidden = true;
      });
      await page.screenshot({ path: path.join(dir, name, "screenshot.png") });
      console.log(`${name}: screenshot.png`);
    }

    await page.close();
  }

  await browser.close();
  process.exit(failed ? 1 : 0);
}

main();
