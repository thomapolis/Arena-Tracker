#ifndef DRAFTHANDLER_H
#define DRAFTHANDLER_H

#include "Widgets/ui_extended.h"
#include "deckhandler.h"
#include "Cards/draftcard.h"
#include "utility.h"
#include "Widgets/draftherowindow.h"
#include "Widgets/draftscorewindow.h"
#include "Widgets/draftmechanicswindow.h"
#include "synergyhandler.h"
#include "twitchhandler.h"
#include <QObject>
#include <QFutureWatcher>


#define CAPTUREDRAFT_START_TIME         1500
#define CAPTUREDRAFT_LOOP_TIME          100
#define CAPTUREDRAFT_LOOP_TIME_FADING   200
#define CAPTUREDRAFT_LOOP_FLANN_TIME    200

#define CARD_ACCEPTED_THRESHOLD             0.35
#define CARD_ACCEPTED_THRESHOLD_INCREASE    0.02
#define CAPTURE_MIN_CANDIDATES                 7
#define CAPTURE_EXTENDED_CANDIDATES            15


class LFtier
{
public:
    int score = 0;
    int maxCard = -1;
};

class ScreenDetection
{
public:
    cv::Rect screenRects[3];
    int screenIndex = -1;
    QPointF screenScale = QPointF(0,0);
};

class DraftHandler : public QObject
{
    Q_OBJECT
public:
    DraftHandler(QObject *parent, Ui::Extended *ui);
    ~DraftHandler();

//Variables
private:
    bool patreonVersion;
    Ui::Extended *ui;
    SynergyHandler *synergyHandler;
    LavaButton *lavaButton;
    ScoreButton *scoreButtonLF, *scoreButtonHA;
    QMap<QString, int> hearthArenaTiers;
    QMap<QString, LFtier> lightForgeTiers;
    QMap<QString, cv::MatND> cardsHist;
    QStringList cardsDownloading;
    DraftCard draftCards[3];
    //Guarda los mejores candidatos de esta iteracion
    QMap<QString, DraftCard> draftCardMaps[3];  //[Code(_premium)] --> DraftCard
    //Se crea al final de la iteracion para ordenar los candidatos por match score
    QMap<double, QString> bestMatchesMaps[3];   //[Match] --> Code(_premium)
    bool cardDetected[3];
    QString arenaHero;
    int deckRatingHA, deckRatingLF;
    cv::Rect screenRects[3];
    int screenIndex;
    int numCaptured;
    bool drafting, heroDrafting, capturing, leavingArena;
    bool mouseInApp;
    Transparency transparency;
    DraftHeroWindow *draftHeroWindow;
    DraftScoreWindow *draftScoreWindow;
    DraftMechanicsWindow * draftMechanicsWindow;
    bool showDraftScoresOverlay, showDraftMechanicsOverlay;
    bool learningMode;
    QString justPickedCard; //Evita doble pick card en Arena.log
    DraftMethod draftMethod;
    QFutureWatcher<ScreenDetection> futureFindScreenRects;
    QLabel *labelLFscore[3];
    QLabel *labelHAscore[3];
    QComboBox *comboBoxCard[3];
    float shownTierScoresHA[3];
    float shownTierScoresLF[3];
    bool extendedCapture;
    bool normalizedLF;
    QStringList heroCodesList;
    QMap<QString, float> heroWinratesMap;
    TwitchHandler *twitchHandler;


//Metodos
private:
    void completeUI();
    cv::MatND getHist(const QString &code);
    cv::MatND getHist(cv::Mat &srcBase);
    void initCodesAndHistMaps(QString hero="");
    void resetTab(bool alreadyDrafting);
    void clearLists(bool keepCounters);
    bool getScreenCardsHist(cv::MatND screenCardsHist[3]);
    void showNewCards(DraftCard bestCards[]);
    void updateDeckScore(float cardRatingHA=0, float cardRatingLF=0);
    bool screenFound();
    ScreenDetection findScreenRects();
    void clearScore(QLabel *label, DraftMethod draftMethod, bool clearText=true);
    void highlightScore(QLabel *label, DraftMethod draftMethod);
    void deleteDraftHeroWindow();
    void deleteDraftScoreWindow();
    void deleteDraftMechanicsWindow();
    void showOverlay();
    void newCaptureDraftLoop(bool delayed=false);
    void updateScoresVisibility();
    void initHearthArenaTiers(const QString &heroString, const bool multiClassDraft);
    QMap<QString, LFtier> initLightForgeTiers(const QString &heroString, const bool multiClassDraft);
    void createDraftWindows(const QPointF &screenScale);
    void mapBestMatchingCodes(cv::MatND screenCardsHist[]);
    double getMinMatch(const QMap<QString, DraftCard> &draftCardMaps);
    bool areCardsDetected();
    void buildBestMatchesMaps();
    CardRarity getBestRarity();
    void getBestCards(DraftCard bestCards[3]);
    void addCardHist(QString code, bool premium, bool isHero=false);
    QString degoldCode(QString fileName);
    void createScoreItems();
    void createSynergyHandler();
    bool isGoldCode(QString fileName);
    void connectAllComboBox();
    void clearAndDisconnectAllComboBox();
    void clearAndDisconnectComboBox(int index);
    void initDraftMechanicsWindowCounters();
    void initSynergyCounters(QList<DeckCard> &deckCardList);
    void updateLabelDeckScore(int deckScoreLFNormalized, int deckScoreHA, int numCards);
    void showMessageDeckScore(int deckScoreLFNormalized, int deckScoreHA);
    void updateAvgScoresVisibility();
    void endHeroDraft();
    void showNewHeroes();
    void buildHeroCodesList();
    void createTwitchHandler();
    void deleteTwitchHandler();

public:
    void reHistDownloadedCardImage(const QString &fileNameCode, bool missingOnWeb=false);
    void setMouseInApp(bool value);
    void setTransparency(Transparency value);
    void setShowDraftScoresOverlay(bool value);
    void setShowDraftMechanicsOverlay(bool value);
    void setLearningMode(bool value);
    void redrawAllCards();
    void updateTamCard();
    void setDraftMethod(DraftMethod value);
    void setTheme();
    void craftGoldenCopy(int cardIndex);
    bool isDrafting();
    void deMinimizeScoreWindow();
    QStringList getAllArenaCodes();
    QStringList getAllHeroCodes();
    void debugSynergiesSet(const QString &set, bool onlyCollectible=true);
    void debugSynergiesCode(const QString &code);
    void testSynergies();
    void initSynergyCodes();
    void setHeroWinratesMap(QMap<QString, float> &heroWinratesMap);
    void updateTwitchChatVotes();

signals:
    void checkCardImage(QString code, bool isHero=false);
    void newDeckCard(QString code);
    void draftStarted();
    void draftEnded();
    void downloadStarted();
    void downloadEnded();
    void overlayCardEntered(QString code, QRect rectCard, int maxTop, int maxBottom, bool alignReverse=true);
    void overlayCardLeave();
    void advanceProgressBar(int remaining, QString text);
    void startProgressBar(int maximum, QString text);
    void showMessageProgressBar(QString text, int hideDelay = 5000);
    void itemEnter(QList<DeckCard> &deckCardList, QRect &rectCard, int maxTop=-1, int maxBottom=-1);
    void itemEnterOverlay(QList<DeckCard> &deckCardList, QPoint &originList, int maxLeft=-1, int maxRight=-1);
    void itemLeave();
    void showPremiumDialog();
    void calculateMinimumWidth();
    void pLog(QString line);
    void pDebug(QString line, DebugLevel debugLevel=Normal, QString file="DraftHandler");

public slots:
    void beginDraft(QString hero, QList<DeckCard> deckCardList = QList<DeckCard>());
    void beginHeroDraft();
    void endDraft();
    void endDraftDeleteMechanicsWindow();
    void showNewRatings(float rating1, float rating2, float rating3,
                        float tierScore1, float tierScore2, float tierScore3,
                        int maxCard1, int maxCard2, int maxCard3, DraftMethod draftMethod);
    void pickCard(QString code);
    void enterArena();
    void leaveArena();
    void minimizeScoreWindow();
    void setPremium(bool premium);
    void setNormalizedLF(bool value);
    void updateMinimumHeight();

private slots:
    void captureDraft();
    void finishFindScreenRects();
    void startFindScreenRects();
    void comboBoxChanged();
    void refreshCapturedCards();
    void twitchHandlerConnectionOk(bool ok);
    void twitchHandlerVoteUpdate(int vote1, int vote2, int vote3);
};

#endif // DRAFTHANDLER_H
