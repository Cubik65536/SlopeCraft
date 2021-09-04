#include "BlockListManager.h"

BlockListManager::BlockListManager(QHBoxLayout * _area,
                                   QObject *parent) : QObject(parent)
{
    isApplyingPreset=false;
    area=_area;
    qDebug()<<((area==nullptr)?" 错误！_area为空指针":"");
    QGroupBox * qgb=nullptr;
    QGridLayout * qgl=nullptr;
    TokiBaseColor * tbc=nullptr;
    tbcs.clear();
    //qDebug("方块列表管理者开始创建QGroupBox");
for(uchar baseColor=0;baseColor<64;baseColor++) {
        if(baseColorNames[baseColor].isEmpty())
            break;
        qgb=new QGroupBox(baseColorNames[baseColor]);
        //qDebug("create QGroupBox");
        qgl=new QGridLayout;
        qgb->setLayout(qgl);
        //qDebug("create Layout");
        tbc=new TokiBaseColor(baseColor,qgl);
        //qDebug("create TokiBaseColor");
        area->addWidget(qgb);
        //qDebug("add QGroupBox to layout");
        tbcs.push_back(tbc);

        connect(this,&BlockListManager::translate,tbc,&TokiBaseColor::translate);
        connect(tbc,&TokiBaseColor::userClicked,this,&BlockListManager::receiveClicked);
    }
//qDebug("Manager构造函数完毕");
}

BlockListManager::~BlockListManager() {
    for(uchar i=0;i<tbcs.size();i++)
        delete tbcs[i];
}

void BlockListManager::setVersion(uchar _ver) {
    if(_ver<12||_ver>=18)return;
    TokiBaseColor::mcVer=_ver;
    for(uchar i=0;i<tbcs.size();i++)
        tbcs[i]->versionCheck();
}

void BlockListManager::addBlocks(const QJsonArray & jArray,QString imgDir) {
    std::vector<std::queue<QJsonObject>> tasks;
    tasks.resize(tbcs.size());
    for(ushort i=0;i<tasks.size();i++)
        while(!tasks[i].empty())
            tasks[i].pop();
    imgDir.replace("\\","/");
    QJsonObject temp;
    uchar baseColor;
    qDebug()<<"jArray.size()="<<jArray.size();
    for(unsigned int i=0;i<jArray.size();i++) {
        temp=jArray[i].toObject();

        if(!isValidBlockInfo(temp)) {
            qDebug()<<"出现不合法的json信息："<<temp;
            continue;
        }
        if(!temp.contains("version"))temp["version"]=0;
        if(!temp.contains("idOld"))   temp["idOld"]="";
        if(!temp.contains("needGlass")) temp["needGlass"]=false;
        if(!temp.contains("isGlowing")) temp["isGlowing"]=false;
        if(!temp.contains("icon")) temp["icon"]="";

        baseColor=temp.value("baseColor").toInt();
        tasks[baseColor].push(temp);
    }
    //qDebug("已经将全部的QJsonObject装入多个队列，开始创建控件");
    for(ushort i=0;i<tasks.size();i++) {
        //qDebug()<<"基色"<<i<<"有"<<tasks[i].size()<<"个方块";
        while(!tasks[i].empty()) {
            tbcs[i]->addTokiBlock(tasks[i].front(),imgDir);
            tasks[i].pop();
        }
    }

}

void BlockListManager::applyPreset(const ushort * preset) {
    qDebug("applyPreset");
    isApplyingPreset=true;
        for(ushort i=0;i<tbcs.size();i++) {
            tbcs[i]->setSelected(preset[i]);
            tbcs[i]->checkBox->setChecked(true);
        }
    isApplyingPreset=false;
}

void BlockListManager::setSelected(uchar baseColor,ushort blockSeq) {
    isApplyingPreset=true;
    tbcs[baseColor]->setSelected(blockSeq);
    isApplyingPreset=false;
}

void BlockListManager::setEnabled(uchar baseColor, bool isEnable) {
    isApplyingPreset=true;
    tbcs[baseColor]->checkBox->setChecked(isEnable);
    isApplyingPreset=false;
}

void BlockListManager::receiveClicked() const {
    if(isApplyingPreset)return;
    emit switchToCustom();
}

void BlockListManager::setLabelColors(const QRgb *colors) {
    for(uchar i=0;i<tbcs.size();i++)
        tbcs[i]->makeLabel(colors[i]);
}

void BlockListManager::getEnableList(bool *dest) const {
    for(uchar i=0;i<tbcs.size();i++)
        dest[i]=tbcs[i]->getEnabled();
}
void BlockListManager::getSimpleBlockList(const simpleBlock ** SBL) const {
    for(uchar i=0;i<64;i++) {
        if(i<tbcs.size())
            SBL[i]=tbcs[i]->getTokiBlock()->getSimpleBlock();
        else
            SBL[i]=nullptr;
    }
}
std::vector<const simpleBlock *> BlockListManager::getSimpleBlockList() const {
    std::vector<const simpleBlock *> SBL(64);
    for(uchar i=0;i<64;i++) {
        if(i<tbcs.size())
            SBL[i]=tbcs[i]->getTokiBlock()->getSimpleBlock();
        else
            SBL[i]=nullptr;
    }
    return SBL;
}
std::vector<const TokiBlock*> BlockListManager::getTokiBlockList() const {
    std::vector<const TokiBlock*> TBL(64);
    for(uchar i=0;i<64;i++) {
        if(i<tbcs.size())
            TBL[i]=tbcs[i]->getTokiBlock();
        else
            TBL[i]=nullptr;
    }
    return TBL;
}

std::vector<const QRadioButton*>BlockListManager::getQRadioButtonList() const {
    std::vector<const QRadioButton*> TBL(64);
    for(uchar i=0;i<64;i++) {
        if(i<tbcs.size())
            TBL[i]=tbcs[i]->getTokiBlock()->getTarget();
        else
            TBL[i]=nullptr;
    }
    return TBL;
}

std::vector<ushort> BlockListManager::toPreset() const {
    std::vector<ushort> TBL(64);
    for(uchar i=0;i<64;i++) {
        if(i<tbcs.size())
            TBL[i]=tbcs[i]->getSelected();
        else
            TBL[i]=0;
    }
    return TBL;
}

bool isValidBlockInfo(const QJsonObject & json) {
    return (json.contains("id")&&
         json.contains("nameZH")&&
         json.contains("nameEN")&&
         json.contains("baseColor"));
}

const QString BlockListManager:: baseColorNames[64]={
    "00 None",
    "01 Grass",
    "02 Sand",
    "03 Wool",
    "04 Fire",
    "05 Ice",
    "06 Metal",
    "07 Plant",
    "08 Snow",
    "09 Clay",
    "10 Dirt",
    "11 Stone",
    "12 Water",
    "13 Wood",
    "14 Quartz",
    "15 ColorOrange",
    "16 ColorMagenta",
    "17 ColorLightBlue",
    "18 ColorYellow",
    "19 ColorLime",
    "20 ColorPink",
    "21 ColorGray",
    "22 ColorLightGray",
    "23 ColorCyan",
    "24 ColorPurple",
    "25 ColorBlue",
    "26 ColorBrown",
    "27 ColorGreen",
    "28 ColorRed",
    "29 ColorBlack",
    "30 Gold",
    "31 Diamond",
    "32 Lapis",
    "33 Emerald",
    "34 Podzol",
    "35 Nether",
    "36 TerracottaWhite",
    "37 TerracottaOrange",
    "38 TerracottaMagenta",
    "39 TerracottaLightBlue",
    "40 TerracottaYellow",
    "41 TerracottaLime",
    "42 TerracottaPink",
    "43 TerracottaGray",
    "44 TerracottaLightGray",
    "45 TerracottaCyan",
    "46 TerracottaPurple",
    "47 TerracottaBlue",
    "48 TerracottaBrown",
    "49 TerracottaGreen",
    "50 TerracottaRed",
    "51 TerracottaBlack",
    "52 CrimsonNylium",
    "53 CrimsonStem",
    "54 CrimsonHyphae",
    "55 WarpedNylium",
    "56 WarpedStem",
    "57 WarpedHyphae",
    "58 WarpedWartBlock",
    "59 Deepslate",
    "60 RawIron",
    "61 GlowLichen",
    "",
    ""
};