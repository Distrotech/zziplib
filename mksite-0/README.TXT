* Why does mksite exist?

I have experiences with a lot of doc generator tools. Most of the
sophisticated generator tools need to pre-installed in a system in
order to let your project get documentation. Package do not like
that atleast, and its only acceptable for them if it is a very
common package also used in a lot of other projects. The counter
effect is that they are not very flexible, you can not easily add
your own stuff, and mostly those third party packages have yet
other dependencies that you can not tell to those who want to 
install your package from source.

In the end I came to the conclusion to not like to have a third party
dependency for simple html documentation. In most cases that is just
acceptable. In order to allow for a pleasant web site layout, I did
start to separate a navigation bar as an html snippet and the real
documentation files each in separate pages, and using simple sed
scripts to combine these into the final pages. Be assured that it
did turn out to be sufficient for all the plain documentation of
a project.

The only problem had been that it is only good for rather small
documentation sites that can be limited to a static navigation
bar. Each direct item is listed in the nav bar, and most other parts
won't get a nav bar at all as the resulting navigation is not easy 
and does not have a professional look with feedback and nav buttons 
to click as next and back and up.

* What is mksite then?

The mksite scripting is the natural extension from the above system,
instead of limiting itself to a navigation bar and its items it does
go to the next step by starting off from a `site map` file. Surely
you have seen such a site map file in some web documentation projects.
Here it is used as the input that we make up the navigation items of 
all pages listed in the site map.

Since this is much more complex, it is not anymore sufficient to
copy'n'paste combiner scripts from one project to another. Instead
this helper project exists that provides with small doc generator
scripts that can be copied around. It should be as simple as picking
up a file of rather small size, adding it to your project, building
up a site map file (possibly generated from makefile rules) and let
it run.

I was widely wondering if it is okay to let a scripting language
be a prerequisite for these scripts. After all, I am trying to
build all functionality in a variety of scripting engines, among
them perl, python and shell/sed scripting. Any of these can be
assumed to be already present in a system. Or it would not hurt
to install them from premade packages.

* How does it work?

The real is in the site map file, per default named site.htm or
site.html. That file takes the format of a `table of contents`
or `index overview`, the latter has additional infos along that
tell of the page content.

The script(s) generate one or more navigation bars, creates an
order of pages (for previous/next navigation) and a hierarchic
categorization (group pages). Possibly further pages can be
scanned from referenced pages but that's left to be optional.
After that each real doc page can be bound in order with their
respective navigation items.

From my experiences the real doc pages take the format of normal
html pages, so you have an easy time with editing them and 
having a preview in the browser. The script generator will scan
these pages and detect <title> items and descriptions automatially
for each page. Likewise information from third places an be bound,
like adding dates from file dates or cvs, or resolving unbound
cross links from a link reference page.

* Related work

http://ambient.2y.net/pysite/
