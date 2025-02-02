
#include <QMap>
#include <TEntityResolver.h>
#include <QtTest/QtTest>
#include "utils.h"

class TEntityResolverTest : public QObject {
Q_OBJECT

private:

private slots:

    void initTestCase()
    {
    }

    void testStandardEntities()
    {
        TEntityResolver resolver;

        QCOMPARE(resolver.getResolution("&nbsp;"), " ");
        QCOMPARE(resolver.getResolution("&gt;"), ">");
        QCOMPARE(resolver.getResolution("&lt;"), "<");
        QCOMPARE(resolver.getResolution("&amp;"), "&");
        QCOMPARE(resolver.getResolution("&quot;"), "\"");
        QCOMPARE(resolver.getResolution("&Uacute;"), "Ú");
        QCOMPARE(resolver.getResolution("&uacute;"), "ú");
    }

    void testStandardEntitiesCaseInsensitive()
    {
        TEntityResolver resolver;
        TEntityType type;

        QCOMPARE(resolver.getResolution("&GT;"), ">");
        QCOMPARE(resolver.getResolution("&Lt;"), "<");
        QCOMPARE(resolver.getResolution("&AmP;"), "&");
        QCOMPARE(resolver.getResolution("&QUOT;"), "\"");
        QCOMPARE(resolver.getResolution("&Copy;", true, &type), "©");
        QCOMPARE(type, ENTITY_TYPE_SYSTEM);
    }

    void testDecimalCode()
    {
        TEntityResolver resolver;

        QCOMPARE(resolver.getResolution("&#10;"), "\n");
        QCOMPARE(resolver.getResolution("&#37;"), "%");
        QCOMPARE(resolver.getResolution("&#32;"), " ");
    }

    void testHexCode()
    {
        TEntityResolver resolver;

        QCOMPARE(resolver.getResolution("&#x20;"), " ");
        QCOMPARE(resolver.getResolution("&#x26;"), "&");
        QCOMPARE(resolver.getResolution("&#x2B;"), "+");

    }

    void testRegisteredEntities()
    {
        TEntityResolver resolver;
        TEntityType type;

        // Examples from MXP specification https://www.zuggsoft.com/zmud/mxp.htm#ENTITY
        resolver.registerEntity("&Version;", "6.15");
        resolver.registerEntity("&Start;", "<em>");
        resolver.registerEntity("&End;", "</em>");

        QCOMPARE(resolver.getResolution("&VERSION;"), "6.15");
        QCOMPARE(resolver.getResolution("&Start;"), "<em>");
        QCOMPARE(resolver.getResolution("&end;", true, &type), "</em>");
        QCOMPARE(type, ENTITY_TYPE_CUSTOM);
        // Check if disabling resolution of custom entities works and is properly signalled
        QCOMPARE(resolver.getResolution("&end;", false, &type), "&end;");
        QCOMPARE(type, ENTITY_TYPE_UNKNOWN);
    }

    void testRegisterEntityAsChar()
    {
        TEntityResolver resolver;

        resolver.registerEntity("&symbol;", '@');

        QCOMPARE(resolver.getResolution("&symbol;"), "@");
    }

    void testInvalidRegister()
    {
        TEntityResolver resolver;

        QVERIFY(!resolver.registerEntity("&symbol", '@'));
        QVERIFY(!resolver.registerEntity("symbol", '@'));

        QVERIFY(resolver.registerEntity("&symbol;", '@'));
    }

    void testResolveNonExistentEntity()
    {
        TEntityResolver resolver;

        QCOMPARE(resolver.getResolution("&symbol;"), "&symbol;");
    }

    void testInterpolation()
    {
        TEntityResolver resolver;

        QCOMPARE(resolver.interpolate("2 &lt; 4"), "2 < 4");
        QCOMPARE(resolver.interpolate("2 &Lt; 4"), "2 < 4");
        QCOMPARE(resolver.interpolate("You say &quot;Hello World&quot;"), "You say \"Hello World\"");
    }

    void testCustomInterpolation()
    {
        const QMap<QString, QString> attributes = {
                {qsl("&name;"), qsl("drunk sailor")},
                {qsl("&desc;"), qsl("A drunk sailor is lying here")}
        };

        auto mapping = [attributes](auto& attr) {
            auto ptr = attributes.find(attr);
            return ptr != attributes.end() ? *ptr : attr;
        };

        QCOMPARE(TEntityResolver::interpolate("attack &#39;&name;&#39;|look &#39;&name;&#39;", mapping), "attack &#39;drunk sailor&#39;|look &#39;drunk sailor&#39;");
        QCOMPARE(TEntityResolver::interpolate("desc: '&desc;'", mapping), "desc: 'A drunk sailor is lying here'");
    }

    void cleanupTestCase()
    {
    }
};

#include "TEntityResolverTest.moc"
QTEST_MAIN(TEntityResolverTest)
