// Identify the UE sCellCandidate based on enbSCellFreqCandidateList and ueCaSupportedBandList
UehCommonCtxtEnbSCellFreqCandidateVectorD ueSCellFreqCandidateList;
const RnhRadioConfConfigurationDataPtrD momDataPtr = ueCtxtDataPtr->getCellDataPtr()->getConfigurationDataPtr();
// PCell---------------------------------------------------------------------------------------------------------------------------------------
bool isMfbiSupported = caLicenseManagerPtr->isMfbiLicenseEnabled();
 
//********************* CmccFastTrack start WP5260 && WP5413 ****************
bool isCmccFtMfbiEnhSupported = false;
 
RnhRadioConfEUtranGenericCellIfPtrD cellPtr = momDataPtr->getservingEUtranGenericCell();
if (cellPtr->getFroRef().getFroType() == ELIB_CONST_EUTRANCELLTDD_FRO_TYPE)
{
  S32 zzzTemporary10 = ((RnhRadioConfEUtranCellTDDIfPtrD)cellPtr)->zzzTemporary10;
  if ((zzzTemporary10 != -2000000000) && (zzzTemporary10 & 0x01)&& ueCtxtDataPtr->getSysConstValue(SYS_CONST_CMCC_FT_MFBI_ENH_ENABLED_ID))
  {
    if(ueCtxtDataPtr->getFuncState() == UEH_COMMON_FUNC_STATE_X2_INCOMING_HANDOVER)
    {
      const CellIdT& servingCellId = ueCtxtDataPtr->getCellDataPtr()->getCellId();
      UehCommonCtxtX2HODataD comX2HOData = ueCtxtDataPtr->getComX2HOData();
      const RnhNCellEUtranCellGenericInfoIfD* SourceCellInfoPtr = ueCtxtDataPtr->getFreqInfo().getLookupByCgiHandler()->getEUtranCellInfo(servingCellId, comX2HOData.sourceEcgi);
      S32 sourceCellzzzTemporary57 = SourceCellInfoPtr->getZzzTemporary57();
      if (sourceCellzzzTemporary57 != -2000000000)
        isCmccFtMfbiEnhSupported = true;
    }
    else
    {
      isCmccFtMfbiEnhSupported = true;
    }
  }
}
//********************* CmccFastTrack end WP5260 && WP5413 ****************
 
RnhNCellEUtranGenericFrequencyD pCellGenericFrequency;
RnhNCellFreqBandUtilD::getSourceCellGenericFrequency(momDataPtr->getservingEUtranGenericCell(), pCellGenericFrequency);
U32 pCellUeFreqBand = eutranCapabilitiesPtr->getEutranOperatingBand(pCellUeEarfcn);
S32 disableCaBwCombinationCheck = 0;
U32 pCellId = ueCtxtDataPtr->getCellDataPtr()->getCellId();
 
// --------------------------------------------------------------------------------------------------------------------------------------------
//first scell
const RnhNCellEUtranCellGenericInfoIfD* firstSCellInfoPtr = NULL;
U32 firstScellUeEarfcn = UEH_INVALID_U32;
U32  firstScellFreqBand = UEH_INVALID_U32;
 
if (excludedSCellFreq != UEH_INVALID_U32)
{
  firstScellUeEarfcn  = excludedSCellFreq;
  firstScellFreqBand = eutranCapabilitiesPtr->getEutranOperatingBand(firstScellUeEarfcn);
  firstSCellInfoPtr =
      ueCtxtDataPtr->getFreqInfo().getLookupByCgiHandler()->getEUtranCellInfo(pCellId,
                                                                              getConfiguredScellCgiFromEarfcn(firstScellUeEarfcn));
 
}
 
UehCommonCtxtEnbSCellFreqCandidateVectorD enbSCellFreqCandidateList =
    ueCtxtDataPtr->getCellDataPtr()->getEnbSCellFreqCandidateList();
 
if(ueCaSupportedBandList.empty())
{
  return ueSCellFreqCandidateList;
}
 
bool isFastScellFirstSCellSelection = false;
 
U8 sCellSelectionMode = momDataPtr->getCarrierAggregationFunction()->sCellSelectionMode;
UehUeSCellEvalLicenseTypeE sCellEvalLicenseType = caLicenseManagerPtr->getCaPolicyLicenseType();
 
// Check if it is the first sCell for fast sCell selection
if ((firstSCellCandidateInfoPtr == NULL) &&
    ((momDataPtr->getSystemConstantValue(SYS_CONST_DISABLE_SCELL_A1_MEASUREMENT_REPORT_ID) == 1) ||
        ((sCellEvalLicenseType == BASIC_THREE_CC_LICENSE) &&
            ((sCellSelectionMode == UN_ACK_SIMULTANEOUS_SCELL_SELECTION) || (sCellSelectionMode == ACK_SIMULTANEOUS_SCELL_SELECTION)))))
{
  isFastScellFirstSCellSelection = true;
}
 
U8  numOfSCellOperationDisabled = 0;
U8  numOfSCellAdminUnlock = 0;
U16 numberOfEnbSCellCandidates = getNumberOfSCellsInTheList(enbSCellFreqCandidateList);
ELIB_TRACE_UE_LEVEL2(
    ueCtxtDataPtr->getUeTraceHandlerPtr(),
    ueCtxtDataPtr->getRacUeRef(),
    Ft_CARRIER_AGGREGATION,
    "calculateUeSCellCandidateListCommon enbSCellFreqCandidateList size =%d, numberOfEnbSCellCandidates =%d",
    enbSCellFreqCandidateList.size(), numberOfEnbSCellCandidates);
 
for (UehCommonCtxtEnbSCellFreqCandidateVectorD::iterator freqIter = enbSCellFreqCandidateList.begin();
    freqIter != enbSCellFreqCandidateList.end();
    freqIter++)
{
  /* for each frequency */
  U32 freqId = (*freqIter).freqId;
  UehCommonCtxtScellFreqPriorityInfoE sCellFreqPriority = (*freqIter).sCellFreqPriority;
  UehCommonCtxtEnbSCellCandidateVectorD enbSCellCandidateList = (*freqIter).getSCellCandidateList();
  UehCommonCtxtEnbSCellCandidateVectorD ueSCellCandidateList;
  UehCommonCtxtEnbSCellFreqCandidateD ueSCellFreqCandidate;
  UehCommonCtxtEnbSCellCandidateD ueSCellCandidate;
 
  ueSCellCandidateList.clear();
  ELIB_TRACE_UE_LEVEL2(
      ueCtxtDataPtr->getUeTraceHandlerPtr(),
      ueCtxtDataPtr->getRacUeRef(),
      Ft_CARRIER_AGGREGATION,
      "Jing flag1 calculateUeSCellCandidateListCommon freqId =%d",
      freqId);
  // save the all sCells in this freq in the UE candidate list.
  for(UehCommonCtxtEnbSCellCandidateVectorD::iterator cellIter = enbSCellCandidateList.begin();
      cellIter != enbSCellCandidateList.end();
      cellIter++)
  {   
    // SCell--------------------------------------------------------------------------------------------------------------------------------------------
    const RnhNCellEUtranCellGenericInfoIfD* sCellInfoPtr =
        ueCtxtDataPtr->getFreqInfo().getLookupByCgiHandler()->getEUtranCellInfo(pCellId, cellIter->cgi);
    bool isExternalSCell = false;
   
    if(cellIter->isExternalSCell)
    {
      ELIB_TRACE_UE_LEVEL2(
          ueCtxtDataPtr->getUeTraceHandlerPtr(),
          ueCtxtDataPtr->getRacUeRef(),
          Ft_CARRIER_AGGREGATION,
          "calculateUeSCellCandidateListCommon External SCell Candidate. SCell CGI=%s cellId=%d.",
          cellIter->cgi.toDecimal().c_str(),
          cellIter->cellId);
 
      isExternalSCell = true;
    }
   
    S32 sCellTxModeConfigureInMom = sCellInfoPtr->getConfiguredTxMode();
    S16 sCellTxMode;
    S32 sCellNoOfUsedTxAntenna = UEH_INVALID_S32;
    S32 bandCombinationIndex =-1;
   
    if (isExternalSCell)
    {
      sCellNoOfUsedTxAntenna = sCellInfoPtr->getNoOfUsedTxAntennas();
    }
    else
    {
      sCellNoOfUsedTxAntenna =
          ueCtxtDataPtr->getCellDataPtr()->getServingCellHandlerPtr()->getNoOfUsedTxAntennas(cellIter->cgi.eNodeBCellId.cellId);
    }
 
    // --------------------------------------------------------------------------------------------------------------------------------------------
 
    if(!isSCellCandidateValid(sCellInfoPtr, isExternalSCell, numOfSCellOperationDisabled, numOfSCellAdminUnlock))
    {    
      ELIB_TRACE_UE_LEVEL2(
          ueCtxtDataPtr->getUeTraceHandlerPtr(),
          ueCtxtDataPtr->getRacUeRef(),
          Ft_CARRIER_AGGREGATION,
          "calculateUeSCellCandidateListCommon SCell Candidate is not Enable. SCell CGI=%s isExternalSCell=%d.",
          cellIter->cgi.toDecimal().c_str(),
          isExternalSCell);
     
      continue;
    }
 
    //HU52110
    if(firstSCellInfoPtr != NULL && sCellInfoPtr->getCgi() == firstSCellInfoPtr->getCgi())
    {
       ELIB_TRACE_UE_LEVEL2(
          ueCtxtDataPtr->getUeTraceHandlerPtr(),
          ueCtxtDataPtr->getRacUeRef(),
          Ft_CARRIER_AGGREGATION,
          "calculateUeSCellCandidateListCommon SCell Candidate CGI is the same as first SCell's CGI. SCell Candidate CGI=%s, first SCell CGI=%s.",
          sCellInfoPtr->getCgi().toDecimal().c_str(), firstSCellInfoPtr->getCgi().toDecimal().c_str());
 
       continue;
    }
 
    if (excludedSCellFreq != sCellInfoPtr->getGenericFrequency()->getEarfcn())
    {
      if ((sCellInfoPtr->getIsDlOnly() == 1) ||
          (sCellInfoPtr->getCellBarred() == 0))
      {
        // if dl only cell then dl only license should be valid
        if (!(caLicenseManagerPtr->checkDlSupplemntaryScellLicense()))
        {
          // DL Only license is not operable.
          // moving on to next potential SCell candidate.
          ELIB_TRACE_UE_LEVEL2(
              ueCtxtDataPtr->getUeTraceHandlerPtr(),
              ueCtxtDataPtr->getRacUeRef(),
              Ft_CARRIER_AGGREGATION,
              "calculateUeSCellCandidateListCommon DlOnly service state In-Operable. No SCell CGI=%s"
              "isDlOnly = %d, cellBarred = %d, dlSipplementaryScellLicense = %d",
              cellIter->cgi.toDecimal().c_str(),
              sCellInfoPtr->getIsDlOnly(),
              sCellInfoPtr->getCellBarred(),
              caLicenseManagerPtr->checkDlSupplemntaryScellLicense());
         
          continue;
        }
      }
 
      // if license service state is INOPERABLE, no TDD SCell if the PCell is FDD.
      if(momDataPtr->getservingEUtranGenericCell()->froType == ELIB_CONST_EUTRANCELLFDD_FRO_TYPE)
      {
        // FDD PCELL
        if (sCellInfoPtr->getCellFroRef().getFroType() == ELIB_CONST_EUTRANCELLTDD_FRO_TYPE ||
            sCellInfoPtr->getCellFroRef().getFroType() == ELIB_CONST_EXTERNALEUTRANCELLTDD_FRO_TYPE)
        {
          // TDD SCELL
          // FDD TDD CA license.
          if(ueCtxtDataPtr->isLicenseOperable(ELIB_CONST_LICENSE_CARRIER_AGGREGATION_FDD_TDD_ID) == false)
          {
 
            // FDD TDD CA license service state is not OPERABLE.
            // NO TDD SCell when PCell is FDD
            // Move to next potential SCell candidate        
            ELIB_TRACE_UE_LEVEL2(
                ueCtxtDataPtr->getUeTraceHandlerPtr(),
                ueCtxtDataPtr->getRacUeRef(),
                Ft_CARRIER_AGGREGATION,
                "calculateUeSCellCandidateListCommon FDD TDD CA license service state In-Operable. If PCell is FDD, no TDD SCell CGI=%s, isExternalSCell=%d",
                cellIter->cgi.toDecimal().c_str(),
                isExternalSCell);
 
            continue;
          }
 
          // PCell is FDD, SCell is TDD.
          if(!initialFddPcellTddScellTxModeCalculation(sCellTxMode, sCellTxModeConfigureInMom, sCellNoOfUsedTxAntenna))
          {
            // sCellTxMode is not valid
            // Move to next SCell candidate.
            ELIB_TRACE_UE_LEVEL2(
                ueCtxtDataPtr->getUeTraceHandlerPtr(),
                ueCtxtDataPtr->getRacUeRef(),
                Ft_CARRIER_AGGREGATION,
                "calculateUeSCellCandidateListCommon FDD PCell, TDD SCell Tx Mode is not supported. TDD SCell CGI=%s isExternalSCell=%d",
                cellIter->cgi.toDecimal().c_str(),
                isExternalSCell);
 
            continue;
          }
        }
        else if (sCellInfoPtr->getCellFroRef().getFroType() == ELIB_CONST_EUTRANCELLFDD_FRO_TYPE ||
                 sCellInfoPtr->getCellFroRef().getFroType() == ELIB_CONST_EXTERNALEUTRANCELLFDD_FRO_TYPE)
        {
          // FDD SCELL
          // PCell is FDD, SCell is TDD.
          if(!initialFddPcellFddScellTxModeCalculation(sCellTxMode, sCellTxModeConfigureInMom, sCellNoOfUsedTxAntenna))
          {
            // sCellTxMode is not valid
            // Move to next SCell candidate.
            ELIB_TRACE_UE_LEVEL2(
                ueCtxtDataPtr->getUeTraceHandlerPtr(),
                ueCtxtDataPtr->getRacUeRef(),
                Ft_CARRIER_AGGREGATION,
                "calculateUeSCellCandidateListCommon FDD PCell, FDD SCell Tx Mode is not supported. FDD SCell CGI=%s isExternalSCell=%d",
                cellIter->cgi.toDecimal().c_str(),
                isExternalSCell);
 
            continue;
          }
        }
      }
      else if (momDataPtr->getservingEUtranGenericCell()->froType == ELIB_CONST_EUTRANCELLTDD_FRO_TYPE)
      {
        // for TDD TDD CA, not all the TDD sCellTxModes are supported
        // TDD sCellTxMode check
        // PCell is TDD, SCell is TDD
 
        //TDD PCell TDD SCell,
        //CmccFastTrack WP4913 start
        if (sCellInfoPtr->getCellFroRef().getFroType() == ELIB_CONST_EUTRANCELLTDD_FRO_TYPE)
        {
          if(!initialTddPcellTddScellTxModeCalculation(sCellTxMode, sCellTxModeConfigureInMom, sCellNoOfUsedTxAntenna))
          {
            // sCellTxMode is not valid
            // Move to next SCell candidate.
            ELIB_TRACE_UE_LEVEL2(
              ueCtxtDataPtr->getUeTraceHandlerPtr(),
              ueCtxtDataPtr->getRacUeRef(),
              Ft_CARRIER_AGGREGATION,
              "calculateUeSCellCandidateListCommon TDD SCell Tx Mode and TDD PCell Tx Mode combination is not supported. TDD SCell CGI=%s isExternalSCell=%d",
              cellIter->cgi.toDecimal().c_str(),
              isExternalSCell);
 
            continue;
          }
        }
        else if (sCellInfoPtr->getCellFroRef().getFroType() == ELIB_CONST_EUTRANCELLFDD_FRO_TYPE)
        {
          if(ueCtxtDataPtr->getSysConstValue(SYS_CONST_CMCC_FT_FDD_TDD_CA_TDD_AS_PCELL_ENABLED_ID) == 0)
          {
            // TDD FDD CA SC is 0.
            ELIB_TRACE_UE_LEVEL2(
              ueCtxtDataPtr->getUeTraceHandlerPtr(),
              ueCtxtDataPtr->getRacUeRef(),
              Ft_CARRIER_AGGREGATION,
              "calculateUeSCellCandidateListCommon TDD-FDD CA is not supported. TDD SCell CGI=%s",
              cellIter->cgi.toDecimal().c_str());
            // NO FDD SCell when PCell is TDD
            continue;
          }
          if(!initialTddPcellFddScellTxModeCalculation(sCellTxMode, sCellTxModeConfigureInMom, sCellNoOfUsedTxAntenna))
          {
            // sCellTxMode is not valid
            // Move to next SCell candidate.
            ELIB_TRACE_UE_LEVEL2(
              ueCtxtDataPtr->getUeTraceHandlerPtr(),
              ueCtxtDataPtr->getRacUeRef(),
              Ft_CARRIER_AGGREGATION,
              "calculateUeSCellCandidateListCommon FDD SCell Tx Mode and TDD PCell Tx Mode combination is not supported. TDD SCell CGI=%s",
              cellIter->cgi.toDecimal().c_str());
 
            continue;
          }
        }
        //CmccFastTrack WP4913 end
      }
 
      if (isExternalSCell && sCellInfoPtr->getChannelBandwidth() < 10000)
      {
        ELIB_TRACE_UE_ABNORMAL_UID(0x199d, ueCtxtDataPtr->getCellId(), ueCtxtDataPtr->getRacUeRef(),
                                   "External sCell bandwidth %d is less than 10Mhz. Reject eSCell %s dynamic selection.",
                                   sCellInfoPtr->getChannelBandwidth(), cellIter->cgi.toString().c_str());
        continue;
      }
     
      if (isExternalSCell && (cellIter->cellId == (U32)(-1)))
      {
        ELIB_TRACE_UE_ABNORMAL_UID(0x19bf, ueCtxtDataPtr->getCellId(), ueCtxtDataPtr->getRacUeRef(),
                                   "External sCell cellId %d is invalid ESCell Cgi. Reject eSCell %s dynamic selection.",
                                   cellIter->cellId, cellIter->cgi.toString().c_str());
        continue;
      }
 
      RnhRadioConfEUtranGenericCellIfD* pCellEUtranGenericCellPtr = momDataPtr->getservingEUtranGenericCell();
      RnhNCellEUtranGenericFrequencyRangeD pCellGenericFrequencyRange(&pCellGenericFrequency, RnhNCellUtilD::getEutranNdlBandWidth(pCellEUtranGenericCellPtr));
      RnhNCellEUtranGenericFrequencyRangeD sCellGenericFrequencyRange(sCellInfoPtr->getGenericFrequency(), sCellInfoPtr->getChannelBandwidth());
      if ((ueCtxtDataPtr->getSysConstValue(SYS_CONST_DISABLE_CA_BW_COMBINATION_ID) == 1)||
          (eutranCapabilitiesPtr->is3CcSupported()
              && ueCtxtDataPtr->getSysConstValue(SYS_CONST_ENABLE3C_CA_BW_COMBINATION_ID) == 0))
      {
        disableCaBwCombinationCheck = 1;
      }
      std::pair<U32, SupportedCarrierAggregation> sCellUeFreqBandDlUl = RnhNCellFreqBandUtilD::getCarrierAggregationSCellUeFreqBandCommon(eutranCapabilitiesPtr,
                                                                                              &pCellGenericFrequencyRange,
                                                                                              &sCellGenericFrequencyRange,
                                                                                              disableCaBwCombinationCheck,
                                                                                              ueCtxtDataPtr->getSysConstValue(SYS_CONST_DISABLE_CA_UL_BW_COMBINATION_ID),
                                                                                              isMfbiSupported,
                                                                                              pCellUeFreqBand,
                                                                                              ueCaSupportedBandList,
                                                                                              &bandCombinationIndex,
                                                                                              ueCtxtDataPtr->getSysConstValue(SYS_CONST_CMCC_FT_ENABLE_MFBI_CA_ID),
                                                                                              isCmccFtMfbiEnhSupported);
      U32 sCellUeFreqBand = sCellUeFreqBandDlUl.first;
 
      // In case of fast sCell selection, use the cell info for the first sCell candidate
      if (firstSCellCandidateInfoPtr != NULL)
      {
        firstSCellInfoPtr = firstSCellCandidateInfoPtr;
      }
 
      if ((sCellUeFreqBand != RnhNCellFreqInfoConstantsD::RNH_NCELL_FREQ_INFO_CONSTANTS_BAND_UNDEFINED)
          && (firstSCellInfoPtr != NULL))
      {
        // Check if 2+2 CA is already configured.
        bool twoPlusTwoCaConfigured = ueCtxtDataPtr->getScellConfiguration().hasUlScellconfigureInConfiguredList();
        //extra check for  pcell +scell1 + scell2 are needed for 3C ue
        RnhNCellEUtranGenericFrequencyRangeD firstSCellFrequencyRange(firstSCellInfoPtr->getGenericFrequency(),firstSCellInfoPtr->getChannelBandwidth());
        sCellUeFreqBandDlUl = RnhNCellFreqBandUtilD::getThirdFreqBandForCarrierAggregation (eutranCapabilitiesPtr,
                                                                                        &pCellGenericFrequencyRange,
                                                                                        &firstSCellFrequencyRange,
                                                                                        &sCellGenericFrequencyRange,
                                                                                        pCellUeEarfcn,
                                                                                        pCellUeFreqBand,
                                                                                        firstScellUeEarfcn,
                                                                                        firstScellFreqBand,
                                                                                        disableCaBwCombinationCheck,
                                                                                        ueCtxtDataPtr->getSysConstValue(SYS_CONST_DISABLE_CA_UL_BW_COMBINATION_ID),
                                                                                        isMfbiSupported,
                                                                                        &bandCombinationIndex,
                                                                                        twoPlusTwoCaConfigured);
 
        sCellUeFreqBand = sCellUeFreqBandDlUl.first;
      }
 
      ueSCellCandidate.carrierAggregationSupportedByCandidate = sCellUeFreqBandDlUl.second;
 
      RnhRadioConfEUtranGenericCellIfPtrVectorD::const_iterator nCellIter;
      RnhRadioConfConfigurationDataPtrD configurationPtr = ueCtxtDataPtr->getCellDataPtr()->getConfigurationDataPtr();
 
      for(nCellIter = configurationPtr->nEUtranGenericCellVectorPtr.begin();
          nCellIter != configurationPtr->nEUtranGenericCellVectorPtr.end();
          ++nCellIter)
      {
        if((*nCellIter)->cellId == (S32)(*cellIter).cgi.eNodeBCellId.cellId)
        {
          ueSCellCandidate.ulSCellPriority = (*nCellIter)->ulSCellPriority;
          break;
        }
      }
 
      if (sCellUeFreqBand != RnhNCellFreqInfoConstantsD::RNH_NCELL_FREQ_INFO_CONSTANTS_BAND_UNDEFINED)
      {
        U32 sCellUeEarfcn = sCellGenericFrequencyRange.getGenericFrequency()->getEarfcn(sCellUeFreqBand);
 
        // Make sure scell frequency not same as excluded frequency (first scell frequency).
        // This may happen if MFBI is on and additional frequency band for scell is used.
        if (sCellUeEarfcn != excludedSCellFreq)
        {
          if((sCellUeFreqBand != pCellUeFreqBand) &&
              (eutranCapabilitiesPtr->isUlBandAggregationSupported(pCellUeFreqBand)))
          {
            /* CA inter Band -----  */
            ueSCellCandidate.cgi = (*cellIter).cgi;
            ueSCellCandidate.cellId = (*cellIter).cellId;
            ueSCellCandidate.isExternalSCell = (*cellIter).isExternalSCell;
            ueSCellCandidate.sCellPriority = getSCellCandidatePriority(sCellInfoPtr, pCellId, alg);
 
            if (sCellInfoPtr->getIsDlOnly() != 1)
            {
              // push the SCell candidate in the back
              ueSCellCandidateList.push_back(ueSCellCandidate);
            }
            else
            {
              // isDlOnly Cell add to top of list, select dl only cell first.
              ueSCellCandidateList.insert(ueSCellCandidateList.begin(), ueSCellCandidate);
            }
 
            ELIB_TRACE_UE_LEVEL2(
                ueCtxtDataPtr->getUeTraceHandlerPtr(),
                ueCtxtDataPtr->getRacUeRef(),
                Ft_CARRIER_AGGREGATION,
                "calculateUeSCellCandidateListCommon inter band : CGI=%s cellId=%d freqId=%d sCellFreqPriority=%d sCellPriority=%d bandCombinationIndex=%d isExternalSCell=%d"
                "carrierAggregationSupportedByCandidate = %d",
                ueSCellCandidate.cgi.toDecimal().c_str(),
                ueSCellCandidate.cellId,
                freqId,
                sCellFreqPriority,
                ueSCellCandidate.sCellPriority,
                bandCombinationIndex,
                isExternalSCell,
                ueSCellCandidate.carrierAggregationSupportedByCandidate);
          }
          else if ((sCellUeFreqBand == pCellUeFreqBand) &&
              (pCellUeEarfcn != sCellUeEarfcn))
          {
            /* CA intra band */
            ueSCellCandidate.cgi = (*cellIter).cgi;
            ueSCellCandidate.cellId = (*cellIter).cellId;
            ueSCellCandidate.isExternalSCell = (*cellIter).isExternalSCell;
            ueSCellCandidate.sCellPriority = getSCellCandidatePriority(sCellInfoPtr, pCellId, alg);
 
            if (sCellInfoPtr->getIsDlOnly() != 1)
            {
              // push the SCell candidate in the back
              ueSCellCandidateList.push_back(ueSCellCandidate);
            }
            else
            {
              // isDlOnly Cell add to top of list, select dl only cell first.
              ueSCellCandidateList.insert(ueSCellCandidateList.begin(), ueSCellCandidate);
            }
 
            ELIB_TRACE_UE_LEVEL2(
                ueCtxtDataPtr->getUeTraceHandlerPtr(),
                ueCtxtDataPtr->getRacUeRef(),
                Ft_CARRIER_AGGREGATION,
                "calculateUeSCellCandidateListCommon intra band : CGI=%s cellId=%d freqId %d sCellFreqPriority=%d sCellPriority=%d bandCombinationIndex=%d isExternalSCell =%d"
                "carrierAggregationSupportedByCandidate = %d",
                ueSCellCandidate.cgi.toDecimal().c_str(),
                ueSCellCandidate.cellId,
                freqId,
                sCellFreqPriority,
                ueSCellCandidate.sCellPriority,
                bandCombinationIndex,
                isExternalSCell,
                ueSCellCandidate.carrierAggregationSupportedByCandidate);
          }
        } // exludedSCellFreq
      }
    } // (excludedSCellFreq != sCellInfoPtr->getGenericFrequency()->getEarfcn())
  } // for UehCommonCtxtEnbSCellCandidateVectorD
 
  // prioritized the SCell candidate list based on DlOnly and CoverageIndicator
  // Cmcc Fast Track WP4989 Begin
  if(ueCtxtDataPtr->isSysConstEnabled(SYS_CONST_CMCC_FT_ENABLE_MFBI_CA_ID)||ueCtxtDataPtr->isSysConstEnabled(SYS_CONST_CMCC_FT_DSS_CA_ENABLED_ID))
  {
    ELIB_TRACE_UE_LEVEL2(
        ueCtxtDataPtr->getUeTraceHandlerPtr(),
        ueCtxtDataPtr->getRacUeRef(),
        Ft_CARRIER_AGGREGATION,
        "No need to remove the low prioirized scells");
  }
  else
  {
      ueSCellCandidateList = calculatePrioritizedUeSCellCandidateList(ueSCellCandidateList, alg);//Legacy behavior
  }
  // Cmcc Fast Track WP4989 End
 
  // WP3714 Filter the candidate list further based on Cross Du license
  ueSCellCandidateList = calculateCrossDuUeSCellCandidateList(ueSCellCandidateList);
 
  // Fast sCell selection
  if (isFastScellFirstSCellSelection)
  {
    ueSCellCandidateList = calculatePrioritizedUlUeSCellCandidateList(ueSCellCandidateList, alg);
  }
 
  if(!ueSCellCandidateList.empty())
  {
    ueSCellFreqCandidate.setSCellCandidateList(ueSCellCandidateList);
    ueSCellFreqCandidate.freqId = freqId;
   
    ueSCellFreqCandidate.sCellFreqPriority = sCellFreqPriority;
   
    //add the Freq, TDD SCell freq is first in the list.
    if (sCellFreqPriority == UEH_COMMON_SCELL_FREQ_PRIORITY_TDD)
    {
      ueSCellFreqCandidateList.insert(ueSCellFreqCandidateList.begin(),ueSCellFreqCandidate);
    }
    else // FDD SCell include DlOnly
    {
      ueSCellFreqCandidateList.push_back(ueSCellFreqCandidate);
    }
  }
} /* for each frequency */
 
// start a CA Opportunity Timer when no SCell is selected and
// aLL SCell Candidates OperationalState are disable and
// there is at least one SCell Candidate AdminState is unlocked.
// This is to allow a potential future SCell selection when the OperationState of the unlocked Cell becomes enable after RU restart.
 
ELIB_TRACE_UE_LEVEL2(
    ueCtxtDataPtr->getUeTraceHandlerPtr(),
    ueCtxtDataPtr->getRacUeRef(),
    Ft_CARRIER_AGGREGATION,
    "calculateUeSCellCandidateListCommon: numberOfEnbSCellCandidates =%d  numOfSCellOperationDisabled =%d numOfSCellAdminUnlock =%d ",
    numberOfEnbSCellCandidates,
    numOfSCellOperationDisabled,
    numOfSCellAdminUnlock);
 
if ((isCaOpportunityTimerNeeded(numberOfEnbSCellCandidates, numOfSCellOperationDisabled, numOfSCellAdminUnlock)) &&
    (ueSCellFreqCandidateList.size() == 0))
{
  // start CA Opportunity timer
  measurementTimerHandlerPtr->startScellTimer(ueCtxtDataPtr->getCellDataPtr()->getCellId(),
                                              UEH_UE_SCELL_EVAL_CA_OPPORTUNITY_TIMEOUT_TYPE);
}
 
// Prioritize the commonUeSCellFreqCandidateList based on TDD, FDD and DlOnly
ueSCellFreqCandidateList = calculatePrioritizedUeSCellFreqCandidateList(ueSCellFreqCandidateList, alg);
 
// Fast sCell selection
if (isFastScellFirstSCellSelection)
{
  ueSCellFreqCandidateList = calculatePrioritizedUlUeSCellFreqCandidateList(ueSCellFreqCandidateList, alg);
}
 
return ueSCellFreqCandidateList;