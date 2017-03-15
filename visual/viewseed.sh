cd viewseedresult

sed -e "s/SEED/$1/g" index_template.html > index.html

cd ..

firefox viewseedresult/index.html
