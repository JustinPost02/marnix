#!/usr/bin/env python3

"""Simple blue gate detector.

The script rotates each input image by 90 degrees and then detects blue areas,
which can correspond to the left and right posts of a gate.
"""

from __future__ import annotations

import argparse
from pathlib import Path

import cv2
import numpy as np


# HSV range for blue. These values are intentionally broad so they work as a
# simple first detector and can be tuned later with real images.
LOWER_BLUE = np.array([90, 80, 40], dtype=np.uint8)
UPPER_BLUE = np.array([140, 255, 255], dtype=np.uint8)
IMAGE_EXTENSIONS = {".jpg", ".jpeg", ".png", ".bmp", ".tif", ".tiff"}


def rotate_image_90(image: np.ndarray) -> np.ndarray:
	"""Rotate the image 90 degrees counterclockwise."""
	return cv2.rotate(image, cv2.ROTATE_90_COUNTERCLOCKWISE)


def detect_blue_mask(image: np.ndarray) -> np.ndarray:
	"""Return a binary mask for blue pixels in the image."""
	hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
	mask = cv2.inRange(hsv, LOWER_BLUE, UPPER_BLUE)

	kernel = np.ones((5, 5), np.uint8)
	mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
	mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
	return mask


def draw_blue_regions(image: np.ndarray, mask: np.ndarray) -> np.ndarray:
	"""Draw contours and bounding boxes around detected blue regions."""
	output = image.copy()
	contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

	for contour in contours:
		area = cv2.contourArea(contour)
		if area < 200:
			continue

		x, y, w, h = cv2.boundingRect(contour)
		cv2.rectangle(output, (x, y), (x + w, y + h), (0, 255, 0), 2)
		cv2.drawContours(output, [contour], -1, (0, 0, 255), 2)

	return output


def process_image(image_path: Path, output_dir: Path) -> None:
	"""Rotate an image, detect blue, and save the results."""
	image = cv2.imread(str(image_path))
	if image is None:
		print(f"Skipping unreadable image: {image_path}")
		return

	rotated = rotate_image_90(image)
	mask = detect_blue_mask(rotated)
	detected = draw_blue_regions(rotated, mask)

	output_dir.mkdir(parents=True, exist_ok=True)
	stem = image_path.stem

	cv2.imwrite(str(output_dir / f"{stem}_rotated.png"), rotated)
	cv2.imwrite(str(output_dir / f"{stem}_blue_mask.png"), mask)
	cv2.imwrite(str(output_dir / f"{stem}_blue_detected.png"), detected)

	blue_pixels = int(cv2.countNonZero(mask))
	print(f"Processed {image_path.name}: {blue_pixels} blue pixels detected")


def find_images(input_path: Path) -> list[Path]:
	"""Return all images from a file or a directory."""
	if input_path.is_file():
		return [input_path]

	if input_path.is_dir():
		return sorted(
			path for path in input_path.iterdir() if path.suffix.lower() in IMAGE_EXTENSIONS
		)

	return []


def parse_args() -> argparse.Namespace:
	parser = argparse.ArgumentParser(
		description="Rotate images 90 degrees and detect blue gate regions."
	)
	parser.add_argument(
		"input",
		nargs="?",
		default=Path(__file__).resolve().parent / "images",
		type=Path,
		help="Path to an image or a folder with images.",
	)
	parser.add_argument(
		"-o",
		"--output",
		default=Path(__file__).resolve().parent / "output_gate",
		type=Path,
		help="Folder where the output images are saved.",
	)
	return parser.parse_args()


def main() -> None:
	args = parse_args()
	images = find_images(args.input)

	if not images:
		print(f"No images found in: {args.input}")
		return

	for image_path in images:
		process_image(image_path, args.output)


if __name__ == "__main__":
	main()
