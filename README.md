# Inkplate Dashboard

Turn your Inkplate into a viewer for comic strips and (motivational?) messages!

Inkplate Dashboard lets you define multiple views that you can flip through using the middle touch button on the Inkplate 6 and Inkplate 10.

At the moment there is a view that lets you browse through images and another view that displays a user-defined text.

## Image/Comics Viewer

The comics module diplays PNG images. The left and right buttons allow you to traverse a folder structure in alphanumerical order. I wrote this to chronologically read through comic strips, but it can of course be used for any kinds of images.

The viewer does not do any kind of on-the-fly resizing, you need to size the images to fit beforehand.

Something like this might do the trick:

```shell
comics_root="/path/to/some/folder"
for year in $(ls -1 ${comics_root}/); do
    if [ ! -d $year ]; then
        mkdir $year
    fi
    for month in $(ls -1 ${comics_root}/${year}/); do
        echo "$year: $month"
        if [ ! -d $year/$month ]; then
            mkdir $year/$month
        fi
        for file in $(ls -1 ${comics_root}/${year}/${month}/); do
            convert -resize 800x600 ${comics_root}/${year}/${month}/${file} ${year}/${month}/${file%.*}.png
        done
    done
done
```

(replace `800x600` with `1200x825` for Inkplate 10)

## Text Viewer

The text viewer shows a text. That is all it does. It is not pretty, it does not support formatting, but at least it wraps the text. It is really more meant as a code example, to give some orientation over how you would write your own view module.

The text module takes a `text` parameter, containing the text it should display.
Optionally you can also configure the font size with the `size` parameter.

## Example Config

The config file goes into the root folder of the SD card. It should be names `config.toml` and is a TOML file, as the name suggests. 

```toml
[general]
modules = [
    { module = "comics", path = "calvin" },
    { module = "text", text = "Hello! You look great today!", size = 12 },
    { module = "comics", path = "kaenguru" }
]
```


